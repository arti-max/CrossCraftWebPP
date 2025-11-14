#pragma once
#include "net/packet/Packet.hpp"

class SetSpawnPositionPacket : public Packet {
public:
    int x, z, yaw;
    short y;

    SetSpawnPositionPacket() : Packet(PacketType::SET_SPAWN_POSITION) {}

    void serialize() override {
        // Only for server
    }

    void deserialize() override {
        x = readInt();
        y = readShort();
        z = readInt();
        yaw = readInt();
    }
};
