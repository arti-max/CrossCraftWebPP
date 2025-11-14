#pragma once
#include "net/packet/Packet.hpp"

class RequestSpawnPositionPacket : public Packet {
public:
    RequestSpawnPositionPacket() : Packet(PacketType::REQUEST_SPAWN_POSITION) {}

    void serialize() override {
        writeByte(0x00); 
    }

    void deserialize() override {
    }
};
