#pragma once
#include <cstdint>

enum class PacketType : uint8_t {
    // from Client
    LOGIN = 0x00,
    POSITION_UPDATE = 0x01,
    BLOCK_CHANGE = 0x02,
    CHAT_MESSAGE = 0x03,
    
    // from Server
    LOGIN_RESPONSE = 0x10,
    LEVEL_INIT = 0x11,
    LEVEL_DATA = 0x12,
    LEVEL_FINALIZE = 0x13,
    PLAYER_SPAWN = 0x14,
    PLAYER_DESPAWN = 0x15,
    PLAYER_POSITION = 0x16,
    BLOCK_UPDATE = 0x17,
    CHAT_BROADCAST = 0x18,
    
    // Other
    PING = 0xFE,
    DISCONNECT = 0xFF
};

const char* packetTypeToString(PacketType type);
