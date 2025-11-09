#pragma once
#include "net/packet/Packet.hpp"

class BlockUpdatePacket : public Packet {
public:
    int32_t x, y, z;
    uint8_t blockType;

    BlockUpdatePacket() : Packet(PacketType::BLOCK_UPDATE) {}
    
    void serialize() override {
        // Only for server
    }

    void deserialize() override {
        x = readInt();
        y = readInt();
        z = readInt();
        blockType = readByte();
    }
};