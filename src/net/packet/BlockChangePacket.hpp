#pragma once
#include "net/packet/Packet.hpp"

class BlockChangePacket : public Packet {
public:
    int32_t x, y, z;
    uint8_t blockType;
    bool placed;

    BlockChangePacket() : Packet(PacketType::BLOCK_CHANGE) {}
    
    BlockChangePacket(int32_t x, int32_t y, int32_t z, uint8_t blockType, bool placed)
        : Packet(PacketType::BLOCK_CHANGE), x(x), y(y), z(z), blockType(blockType), placed(placed) {}

    void serialize() override {
        writeInt(x);
        writeInt(y);
        writeInt(z);
        writeByte(blockType);
        writeByte(placed ? 1 : 0);
    }

    void deserialize() override {
        x = readInt();
        y = readInt();
        z = readInt();
        blockType = readByte();
        placed = readByte() != 0;
    }
};
