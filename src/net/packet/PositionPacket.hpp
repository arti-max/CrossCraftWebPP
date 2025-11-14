#pragma once
#include "net/packet/Packet.hpp"

class PositionPacket : public Packet {
public:
    int8_t playerId;
    float x, y, z;
    float yaw, pitch;

    PositionPacket() : Packet(PacketType::PLAYER_POSITION) {}
    
    PositionPacket(float x, float y, float z, float yaw, float pitch)
        : Packet(PacketType::PLAYER_POSITION), playerId(0), x(x), y(y), z(z), yaw(yaw), pitch(pitch) {}

    PositionPacket(int8_t id, float x, float y, float z, float yaw, float pitch)
        : Packet(PacketType::PLAYER_POSITION), playerId(id), x(x), y(y), z(z), yaw(yaw), pitch(pitch) {}

    void serialize() override {
        writeByte(playerId);
        writeFloat(x);
        writeFloat(y);
        writeFloat(z);
        writeFloat(yaw);
        writeFloat(pitch);
    }

    void deserialize() override {
        playerId = readByte();
        x = readFloat();
        y = readFloat();
        z = readFloat();
        yaw = readFloat();
        pitch = readFloat();
    }
};
