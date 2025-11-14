#pragma once
#include "net/packet/Packet.hpp"
#include <string>

class SpawnPlayerPacket : public Packet {
public:
    int8_t playerId;
    std::string username;
    float x, y, z;
    float yaw, pitch;

    SpawnPlayerPacket() : Packet(PacketType::PLAYER_SPAWN) {}
    
    SpawnPlayerPacket(int8_t id, std::string name, float x, float y, float z, float yaw, float pitch)
        : Packet(PacketType::PLAYER_SPAWN), playerId(id), username(name), x(x), y(y), z(z), yaw(yaw), pitch(pitch) {}

    void serialize() override {
        // Only for server
    }

    void deserialize() override {
        playerId = readByte();
        username = readString();
        x = readFloat();
        y = readFloat();
        z = readFloat();
        yaw = readFloat();
        pitch = readFloat();
    }
};
