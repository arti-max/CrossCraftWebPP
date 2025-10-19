#!/usr/bin/env python3
"""
Классы пакетов для CrossCraft сервера
"""

import struct
import io
from enum import IntEnum

class PacketType(IntEnum):
    """Типы пакетов"""
    # Клиент -> Сервер
    LOGIN = 0x00
    POSITION_UPDATE = 0x01
    BLOCK_CHANGE = 0x02
    CHAT_MESSAGE = 0x03
    
    # Сервер -> Клиент
    LOGIN_RESPONSE = 0x10
    LEVEL_INIT = 0x11
    LEVEL_DATA = 0x12
    LEVEL_FINALIZE = 0x13
    PLAYER_SPAWN = 0x14
    PLAYER_DESPAWN = 0x15
    PLAYER_POSITION = 0x16
    BLOCK_UPDATE = 0x17
    CHAT_BROADCAST = 0x18
    
    # Служебные
    PING = 0xFE
    DISCONNECT = 0xFF

class Packet:
    """Базовый класс пакета"""
    
    def __init__(self, packet_type: PacketType):
        self.type = packet_type
        self.data = io.BytesIO()
    
    def write_byte(self, value: int):
        """Записать байт"""
        self.data.write(struct.pack('!B', value))
    
    def write_short(self, value: int):
        """Записать short (2 байта)"""
        self.data.write(struct.pack('!h', value))
    
    def write_int(self, value: int):
        """Записать int (4 байта)"""
        self.data.write(struct.pack('!i', value))
    
    def write_float(self, value: float):
        """Записать float (4 байта)"""
        self.data.write(struct.pack('!f', value))
    
    def write_string(self, value: str):
        """Записать строку (length + data)"""
        encoded = value.encode('utf-8')
        self.write_short(len(encoded))
        self.data.write(encoded)
    
    def read_byte(self, data: bytes, offset: int) -> tuple:
        """Прочитать байт, возвращает (значение, новый offset)"""
        value = struct.unpack('!B', data[offset:offset+1])[0]
        return value, offset + 1
    
    def read_short(self, data: bytes, offset: int) -> tuple:
        """Прочитать short"""
        value = struct.unpack('!h', data[offset:offset+2])[0]
        return value, offset + 2
    
    def read_int(self, data: bytes, offset: int) -> tuple:
        """Прочитать int"""
        value = struct.unpack('!i', data[offset:offset+4])[0]
        return value, offset + 4
    
    def read_float(self, data: bytes, offset: int) -> tuple:
        """Прочитать float"""
        value = struct.unpack('!f', data[offset:offset+4])[0]
        return value, offset + 4
    
    def read_string(self, data: bytes, offset: int) -> tuple:
        """Прочитать строку"""
        length, offset = self.read_short(data, offset)
        value = data[offset:offset+length].decode('utf-8')
        return value, offset + length
    
    def to_bytes(self) -> bytes:
        """Сериализовать пакет"""
        result = struct.pack('!B', self.type)
        result += self.data.getvalue()
        return result
    
    @staticmethod
    def from_bytes(data: bytes):
        """Десериализовать пакет"""
        packet_type = PacketType(data[0])
        
        if packet_type == PacketType.LOGIN:
            return LoginPacket.from_bytes(data)
        elif packet_type == PacketType.POSITION_UPDATE:
            return PositionPacket.from_bytes(data)
        elif packet_type == PacketType.BLOCK_CHANGE:
            return BlockChangePacket.from_bytes(data)
        elif packet_type == PacketType.CHAT_MESSAGE:
            return ChatPacket.from_bytes(data)
        else:
            print(f"Unknown packet type: {packet_type}")
            return None

class LoginPacket(Packet):
    """Пакет авторизации"""
    
    def __init__(self, username: str = "", session_id: str = "", protocol_version: int = 1):
        super().__init__(PacketType.LOGIN)
        self.username = username
        self.session_id = session_id
        self.protocol_version = protocol_version
    
    def serialize(self):
        """Сериализовать"""
        self.write_byte(self.protocol_version)
        self.write_string(self.username)
        self.write_string(self.session_id)
    
    @staticmethod
    def from_bytes(data: bytes):
        """Десериализовать"""
        packet = LoginPacket()
        offset = 1  # Пропускаем тип пакета
        
        packet.protocol_version, offset = packet.read_byte(data, offset)
        packet.username, offset = packet.read_string(data, offset)
        packet.session_id, offset = packet.read_string(data, offset)
        
        return packet

class LoginResponsePacket(Packet):
    """Ответ на авторизацию"""
    
    def __init__(self, success: bool = True, player_id: int = 0, message: str = ""):
        super().__init__(PacketType.LOGIN_RESPONSE)
        self.success = success
        self.player_id = player_id
        self.message = message
    
    def serialize(self):
        """Сериализовать"""
        self.write_byte(1 if self.success else 0)
        self.write_int(self.player_id)
        self.write_string(self.message)

class PositionPacket(Packet):
    """Пакет позиции"""
    
    def __init__(self, x: float = 0, y: float = 0, z: float = 0, yaw: float = 0, pitch: float = 0):
        super().__init__(PacketType.POSITION_UPDATE)
        self.x = x
        self.y = y
        self.z = z
        self.yaw = yaw
        self.pitch = pitch
    
    def serialize(self):
        """Сериализовать"""
        self.write_float(self.x)
        self.write_float(self.y)
        self.write_float(self.z)
        self.write_float(self.yaw)
        self.write_float(self.pitch)
    
    @staticmethod
    def from_bytes(data: bytes):
        """Десериализовать"""
        packet = PositionPacket()
        offset = 1
        
        packet.x, offset = packet.read_float(data, offset)
        packet.y, offset = packet.read_float(data, offset)
        packet.z, offset = packet.read_float(data, offset)
        packet.yaw, offset = packet.read_float(data, offset)
        packet.pitch, offset = packet.read_float(data, offset)
        
        return packet

class PlayerPositionPacket(Packet):
    """Позиция другого игрока"""
    
    def __init__(self, player_id: int = 0, x: float = 0, y: float = 0, z: float = 0, yaw: float = 0, pitch: float = 0):
        super().__init__(PacketType.PLAYER_POSITION)
        self.player_id = player_id
        self.x = x
        self.y = y
        self.z = z
        self.yaw = yaw
        self.pitch = pitch
    
    def serialize(self):
        """Сериализовать"""
        self.write_int(self.player_id)
        self.write_float(self.x)
        self.write_float(self.y)
        self.write_float(self.z)
        self.write_float(self.yaw)
        self.write_float(self.pitch)

class BlockChangePacket(Packet):
    """Пакет изменения блока"""
    
    def __init__(self, x: int = 0, y: int = 0, z: int = 0, block_type: int = 0, placed: bool = True):
        super().__init__(PacketType.BLOCK_CHANGE)
        self.x = x
        self.y = y
        self.z = z
        self.block_type = block_type
        self.placed = placed
    
    def serialize(self):
        """Сериализовать"""
        self.write_int(self.x)
        self.write_int(self.y)
        self.write_int(self.z)
        self.write_byte(self.block_type)
        self.write_byte(1 if self.placed else 0)
    
    @staticmethod
    def from_bytes(data: bytes):
        """Десериализовать"""
        packet = BlockChangePacket()
        offset = 1
        
        packet.x, offset = packet.read_int(data, offset)
        packet.y, offset = packet.read_int(data, offset)
        packet.z, offset = packet.read_int(data, offset)
        packet.block_type, offset = packet.read_byte(data, offset)
        placed_byte, offset = packet.read_byte(data, offset)
        packet.placed = placed_byte != 0
        
        return packet

class BlockUpdatePacket(Packet):
    """Обновление блока от сервера"""
    
    def __init__(self, x: int = 0, y: int = 0, z: int = 0, block_type: int = 0):
        super().__init__(PacketType.BLOCK_UPDATE)
        self.x = x
        self.y = y
        self.z = z
        self.block_type = block_type
    
    def serialize(self):
        """Сериализовать"""
        self.write_int(self.x)
        self.write_int(self.y)
        self.write_int(self.z)
        self.write_byte(self.block_type)

class ChatPacket(Packet):
    """Чат сообщение"""
    
    def __init__(self, message: str = ""):
        super().__init__(PacketType.CHAT_MESSAGE)
        self.message = message
    
    def serialize(self):
        """Сериализовать"""
        self.write_string(self.message)
    
    @staticmethod
    def from_bytes(data: bytes):
        """Десериализовать"""
        packet = ChatPacket()
        offset = 1
        packet.message, offset = packet.read_string(data, offset)
        return packet

class ChatBroadcastPacket(Packet):
    """Чат сообщение от сервера"""
    
    def __init__(self, sender: str = "", message: str = ""):
        super().__init__(PacketType.CHAT_BROADCAST)
        self.sender = sender
        self.message = message
    
    def serialize(self):
        """Сериализовать"""
        self.write_string(self.sender)
        self.write_string(self.message)

class PlayerSpawnPacket(Packet):
    """Спавн игрока"""
    
    def __init__(self, player_id: int = 0, username: str = "", x: float = 0, y: float = 0, z: float = 0):
        super().__init__(PacketType.PLAYER_SPAWN)
        self.player_id = player_id
        self.username = username
        self.x = x
        self.y = y
        self.z = z
    
    def serialize(self):
        """Сериализовать"""
        self.write_int(self.player_id)
        self.write_string(self.username)
        self.write_float(self.x)
        self.write_float(self.y)
        self.write_float(self.z)

class PlayerDespawnPacket(Packet):
    """Деспавн игрока"""
    
    def __init__(self, player_id: int = 0):
        super().__init__(PacketType.PLAYER_DESPAWN)
        self.player_id = player_id
    
    def serialize(self):
        """Сериализовать"""
        self.write_int(self.player_id)
