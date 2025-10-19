#pragma once
#include "net/packet/Packet.hpp"

class PositionPacket : public Packet {
public:
    float x, y, z;
    float yaw, pitch;

    PositionPacket() : Packet(PacketType::POSITION_UPDATE) {}
    
    PositionPacket(float x, float y, float z, float yaw, float pitch)
        : Packet(PacketType::POSITION_UPDATE), x(x), y(y), z(z), yaw(yaw), pitch(pitch) {}

    void serialize() override {
        writeFloat(x);
        writeFloat(y);
        writeFloat(z);
        writeFloat(yaw);
        writeFloat(pitch);
    }

    void deserialize() override {
        x = readFloat();
        y = readFloat();
        z = readFloat();
        yaw = readFloat();
        pitch = readFloat();
    }
};
