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
    SERVER_IDENTIFICATION = 0x11,
    LEVEL_DATA = 0x12,
    BLOCK_UPDATE = 0x17,
    CHAT_BROADCAST = 0x18,
    PLAYER_SPAWN = 0x20,
    PLAYER_DESPAWN = 0x22,
    PLAYER_POSITION = 0x21,
    SET_SPAWN_POSITION = 0x23,
    REQUEST_SPAWN_POSITION = 0x24,

    SERVER_CHAT_MESSAGE = 0x30,
    CLIENT_CHAT_MESSAGE = 0x31,

    
    // Other
    PING = 0xFE,
    DISCONNECT = 0xFF
};

const char* packetTypeToString(PacketType type);
