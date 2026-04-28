#pragma once
#include "net/packet/Packet.hpp"

class RequestLevelDataPacket : public Packet {
public:
    RequestLevelDataPacket() : Packet(PacketType::REQUEST_LEVEL_DATA) {}

    void serialize() override {
        writeByte(0x00); 
    }

    void deserialize() override {
    }
};
