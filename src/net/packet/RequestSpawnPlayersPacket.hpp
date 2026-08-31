#pragma once

#pragma once
#include "net/packet/Packet.hpp"

class RequestSpawnplayersPacket : public Packet {
public:
    RequestSpawnplayersPacket() : Packet(PacketType::REQUEST_PLAYERS) {}

    void serialize() override {
        writeByte(0x00); 
    }

    void deserialize() override {
    }
};
