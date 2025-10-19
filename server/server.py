#!/usr/bin/env python3
"""
Тестовый WebSocket сервер для CrossCraft
"""

import asyncio
import websockets
from websockets.server import serve
from packet import *
from player import Player
import sys

class CrossCraftServer:
    """Сервер CrossCraft"""
    
    def __init__(self, host: str = "localhost", port: int = 25565):
        self.host = host
        self.port = port
        self.players = {}  # websocket -> Player
        self.level = None
        
        print("=" * 60)
        print("🎮 CrossCraft Test Server")
        print("=" * 60)
        print(f"Host: {host}")
        print(f"Port: {port}")
        print("=" * 60)
    
    async def handle_client(self, websocket, path):
        """Обработка подключения клиента"""
        client_addr = websocket.remote_address
        print(f"[CONNECT] New connection from {client_addr}")
        
        try:
            async for message in websocket:
                await self.handle_packet(websocket, message)
        
        except websockets.exceptions.ConnectionClosed:
            print(f"[DISCONNECT] {client_addr} disconnected")
        
        finally:
            # Удаляем игрока при отключении
            if websocket in self.players:
                player = self.players[websocket]
                print(f"[LEAVE] {player.username} left the game")
                
                # Уведомляем других игроков
                despawn = PlayerDespawnPacket(player.id)
                despawn.serialize()
                await self.broadcast(despawn.to_bytes(), exclude=websocket)
                
                del self.players[websocket]
    
    async def handle_packet(self, websocket, data: bytes):
        """Обработка входящего пакета"""
        print(f"[DEBUG] Received {len(data)} bytes: {data.hex()}")
        
        try:
            packet = Packet.from_bytes(data)
        except Exception as e:
            print(f"[ERROR] Failed to parse packet: {e}")
            import traceback
            traceback.print_exc()
            return
        
        if packet is None:
            print(f"[ERROR] Failed to parse packet")
            return
        
        # Обработка пакетов
        if isinstance(packet, LoginPacket):
            await self.handle_login(websocket, packet)
        
        elif isinstance(packet, PositionPacket):
            await self.handle_position(websocket, packet)
        
        elif isinstance(packet, BlockChangePacket):
            await self.handle_block_change(websocket, packet)
        
        elif isinstance(packet, ChatPacket):
            await self.handle_chat(websocket, packet)
        
        else:
            print(f"[WARNING] Unknown packet type: {packet.type}")
    
    async def handle_login(self, websocket, packet: LoginPacket):
        """Обработка авторизации"""
        print(f"[LOGIN] User: {packet.username}, Session: {packet.session_id}")
        
        # Создаём игрока
        player = Player(websocket, packet.username, packet.session_id)
        self.players[websocket] = player
        
        # Отправляем ответ
        response = LoginResponsePacket(True, player.id, "Welcome to CrossCraft!")
        response.serialize()
        await websocket.send(response.to_bytes())
        
        print(f"[JOIN] {player.username} joined the game (id={player.id})")
        
        # Уведомляем других игроков о новом игроке
        spawn = PlayerSpawnPacket(player.id, player.username, player.x, player.y, player.z)
        spawn.serialize()
        await self.broadcast(spawn.to_bytes(), exclude=websocket)
        
        # Отправляем новому игроку список уже подключённых игроков
        for other_ws, other_player in self.players.items():
            if other_ws != websocket:
                spawn = PlayerSpawnPacket(other_player.id, other_player.username, 
                                        other_player.x, other_player.y, other_player.z)
                spawn.serialize()
                await websocket.send(spawn.to_bytes())
        
        # Отправляем приветственное сообщение
        welcome = ChatBroadcastPacket("Server", f"{player.username} joined the game!")
        welcome.serialize()
        await self.broadcast(welcome.to_bytes())
    
    async def handle_position(self, websocket, packet: PositionPacket):
        """Обработка обновления позиции"""
        if websocket not in self.players:
            return
        
        player = self.players[websocket]
        player.update_position(packet.x, packet.y, packet.z, packet.yaw, packet.pitch)
        
        # Отправляем позицию другим игрокам
        pos_packet = PlayerPositionPacket(player.id, packet.x, packet.y, packet.z, 
                                         packet.yaw, packet.pitch)
        pos_packet.serialize()
        await self.broadcast(pos_packet.to_bytes(), exclude=websocket)
    
    async def handle_block_change(self, websocket, packet: BlockChangePacket):
        """Обработка изменения блока"""
        if websocket not in self.players:
            return
        
        player = self.players[websocket]
        action = "placed" if packet.placed else "broke"
        print(f"[BLOCK] {player.username} {action} block at ({packet.x}, {packet.y}, {packet.z})")
        
        # Отправляем обновление всем игрокам (включая отправителя для подтверждения)
        block_type = packet.block_type if packet.placed else 0
        update = BlockUpdatePacket(packet.x, packet.y, packet.z, block_type)
        update.serialize()
        await self.broadcast(update.to_bytes())
    
    async def handle_chat(self, websocket, packet: ChatPacket):
        """Обработка чат сообщения"""
        if websocket not in self.players:
            return
        
        player = self.players[websocket]
        print(f"[CHAT] <{player.username}> {packet.message}")
        
        # Отправляем сообщение всем игрокам
        broadcast = ChatBroadcastPacket(player.username, packet.message)
        broadcast.serialize()
        await self.broadcast(broadcast.to_bytes())
    
    async def broadcast(self, data: bytes, exclude=None):
        """Отправить пакет всем игрокам"""
        for websocket in self.players.keys():
            if websocket != exclude:
                try:
                    await websocket.send(data)
                except:
                    pass
    
    async def start(self):
        """Запустить сервер"""
        async with serve(self.handle_client, self.host, self.port):
            print(f"\n✅ Server started on ws://{self.host}:{self.port}")
            print("Waiting for connections...\n")
            await asyncio.Future()  # Run forever

def main():
    """Главная функция"""
    import argparse
    
    parser = argparse.ArgumentParser(description='CrossCraft Test Server')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=25565, help='Server port')
    
    args = parser.parse_args()
    
    server = CrossCraftServer(args.host, args.port)
    
    try:
        asyncio.run(server.start())
    except KeyboardInterrupt:
        print("\n\n⚠️  Server stopped by user")
        sys.exit(0)

if __name__ == '__main__':
    main()
