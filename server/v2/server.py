import asyncio
import websockets
import struct
import gzip
import logging
from perlin_noise import PerlinNoise
import numpy as np  # ✅ Импортируем numpy

# --- Настройка ---
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
HOST = "localhost"
PORT = 25565
CLIENTS = set()

# --- Константы ---
AIR, STONE, GRASS, DIRT, WATER, SAND = 0, 1, 2, 3, 9, 12
WIDTH, HEIGHT, DEPTH = 256, 256, 64

# ✅ Переменная для хранения сгенерированного мира
WORLD_DATA_COMPRESSED = None

# --- Утилиты ---
def write_short(val): return struct.pack('!h', val)
def write_int(val): return struct.pack('!i', val)

# --- ✅ УСКОРЕННАЯ генерация мира с numpy ---
def generate_world_data_once():
    global WORLD_DATA_COMPRESSED
    if WORLD_DATA_COMPRESSED is not None:
        logging.info("World already generated, using cached version.")
        return

    logging.info("Starting initial world generation with numpy...")
    
    noise = PerlinNoise(octaves=4, seed=1)
    
    # Создаем 2D-массив координат (x, z) для numpy
    x_coords = np.linspace(0, 5, WIDTH, endpoint=False)
    z_coords = np.linspace(0, 5, HEIGHT, endpoint=False)
    xz_grid = np.array([[noise([x, z]) for z in z_coords] for x in x_coords])

    # Вычисляем высоту ландшафта для всей карты разом
    water_level = DEPTH // 3
    terrain_height = (water_level + xz_grid * (DEPTH / 3)).astype(int)
    
    # Создаем 3D-массив мира
    level_data = np.zeros((WIDTH, HEIGHT, DEPTH), dtype=np.uint8)
    
    # Заполняем мир с помощью масок numpy (гораздо быстрее циклов)
    y_indices = np.arange(DEPTH).reshape(1, 1, DEPTH)
    
    level_data[y_indices <= terrain_height[:, :, np.newaxis]] = STONE
    level_data[y_indices == terrain_height[:, :, np.newaxis]] = GRASS
    level_data[np.logical_and(y_indices > terrain_height[:, :, np.newaxis] - 4, y_indices < terrain_height[:, :, np.newaxis])] = DIRT
    level_data[np.logical_and(y_indices > terrain_height[:, :, np.newaxis], y_indices <= water_level)] = WATER
    level_data[np.logical_and(y_indices <= water_level, y_indices == terrain_height[:, :, np.newaxis])] = SAND

    # Пещеры все еще требуют циклов, но это можно оптимизировать позже или убрать для скорости
    # (Для максимальной скорости можно пока закомментировать этот блок)
    cave_noise = PerlinNoise(octaves=8, seed=2)
    for x in range(WIDTH):
        for z in range(HEIGHT):
            for y in range(DEPTH):
                if level_data[x, z, y] == STONE:
                    if cave_noise([x / 50, y / 50, z / 50]) > 0.35:
                        level_data[x, z, y] = AIR

    # Транспонируем массив, чтобы он соответствовал C-порядку (y, z, x)
    final_data = level_data.transpose(2, 1, 0).flatten()

    logging.info("World generation complete. Compressing...")
    WORLD_DATA_COMPRESSED = gzip.compress(final_data.tobytes())
    logging.info(f"World cached. Compressed size: {len(WORLD_DATA_COMPRESSED)} bytes")

# --- Асинхронные функции ---
# ... (register, unregister, broadcast, send_login_response без изменений) ...

async def send_level_data(websocket):
    if WORLD_DATA_COMPRESSED is None:
        logging.error("World data is not generated!")
        return

    logging.info("Sending cached world data...")
    header = write_short(WIDTH) + write_short(HEIGHT) + write_short(DEPTH) + write_int(len(WORLD_DATA_COMPRESSED))
    packet = struct.pack('!B', 0x12) + header + WORLD_DATA_COMPRESSED
    
    await websocket.send(packet)
    logging.info(f"-> SENT LEVEL_DATA (0x12), size: {len(packet)} bytes")

# ... (handler без изменений, он просто вызывает send_level_data) ...
def register(websocket):
    CLIENTS.add(websocket)
    logging.info(f"Client registered. Total clients: {len(CLIENTS)}")

def unregister(websocket):
    CLIENTS.remove(websocket)
    logging.info(f"Client unregistered. Total clients: {len(CLIENTS)}")

async def broadcast(message, exclude_ws=None):
    if CLIENTS:
        # ✅ Создаем задачи из корутин
        tasks = [asyncio.create_task(client.send(message)) for client in CLIENTS if client != exclude_ws]
        if tasks:
            logging.info(f"Broadcasting message to {len(tasks)} clients")
            # ✅ Теперь asyncio.wait() будет работать
            await asyncio.wait(tasks)

async def send_login_response(websocket):
    packet_id = 0x10
    # В вашем C++ коде LOGIN_RESPONSE не содержит данных, просто ID
    packet = struct.pack('!B', packet_id)
    await websocket.send(packet)
    logging.info(f"-> SENT LOGIN_RESPONSE (0x10), size: {len(packet)} bytes")
    
async def handler(websocket):
    logging.info(f"Client connected from {websocket.remote_address}")
    register(websocket)
    
    try:
        # 1. Получаем Login Packet
        login_packet_data = await websocket.recv()
        logging.info(f"<- RECV LOGIN (0x00), size: {len(login_packet_data)} bytes")
        
        # 2. Отправляем Login Response
        await send_login_response(websocket)
        
        # 3. Отправляем мир
        await send_level_data(websocket)
        
        # 4. Основной цикл
        async for message in websocket:
            packet_id = message[0]
            logging.info(f"<- RECV packet 0x{packet_id:02x}, size: {len(message)} bytes")

            # ✅ Обработка BLOCK_CHANGE
            if packet_id == 0x02: # BLOCK_CHANGE
                try:
                    # ✅ ИСПРАВЛЕНО: читаем три int, один byte, один bool
                    # '!iiibB' -> int, int, int, byte, bool (4+4+4+1+1 = 14 байт)
                    x, y, z, block_type, placed_byte = struct.unpack('!iiiBb', message[1:])
                    placed = bool(placed_byte)
                    
                    logging.info(f"  Block change request: ({x}, {y}, {z}) -> type {block_type}, placed: {placed}")
                    
                    # Формируем пакет BLOCK_UPDATE (0x17)
                    update_packet_id = 0x17
                    new_block_type = block_type if placed else 0
                    
                    # ✅ Убедимся, что отправляем тоже в правильном формате (iiiB)
                    update_packet_data = struct.pack('!iiiB', x, y, z, new_block_type)
                    update_packet = struct.pack('!B', update_packet_id) + update_packet_data
                    
                    # Рассылаем всем
                    await broadcast(update_packet)
                    logging.info(f"-> BROADCAST BLOCK_UPDATE (0x17) for ({x}, {y}, {z})")

                except struct.error as e:
                    logging.error(f"Failed to unpack BLOCK_CHANGE packet: {e}")

    except websockets.exceptions.ConnectionClosed as e:
        logging.info(f"Client disconnected (code: {e.code}, reason: '{e.reason}')")
    except Exception as e:
        logging.error(f"An error occurred: {e}", exc_info=True)
    finally:
        unregister(websocket)
        
async def main():
    # ✅ Генерируем мир до запуска сервера
    generate_world_data_once()
    
    async with websockets.serve(handler, HOST, PORT):
        logging.info(f"Server started on ws://{HOST}:{PORT}")
        await asyncio.Future()

if __name__ == "__main__":
    asyncio.run(main())
