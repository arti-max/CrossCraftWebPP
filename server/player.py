#!/usr/bin/env python3
"""
Класс игрока на сервере
"""

class Player:
    """Игрок на сервере"""
    
    player_id_counter = 0
    
    def __init__(self, websocket, username: str, session_id: str):
        Player.player_id_counter += 1
        self.id = Player.player_id_counter
        self.websocket = websocket
        self.username = username
        self.session_id = session_id
        
        # Позиция
        self.x = 128.0
        self.y = 68.0
        self.z = 128.0
        self.yaw = 0.0
        self.pitch = 0.0
        
        print(f"Player created: {self.username} (id={self.id})")
    
    def update_position(self, x: float, y: float, z: float, yaw: float, pitch: float):
        """Обновить позицию"""
        self.x = x
        self.y = y
        self.z = z
        self.yaw = yaw
        self.pitch = pitch
    
    def __str__(self):
        return f"Player({self.username}, id={self.id}, pos=({self.x:.1f}, {self.y:.1f}, {self.z:.1f}))"
