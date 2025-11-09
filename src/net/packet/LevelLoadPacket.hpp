#pragma once
#include "net/packet/Packet.hpp"

class LevelLoadPacket : public Packet {
public:
    int16_t width, height, depth;
    std::vector<uint8_t> compressedData;

    LevelLoadPacket() : Packet(PacketType::LEVEL_DATA) {}


    void serialize() override {
        // Only for server
    }

    void deserialize() override {
        width = readShort();
        height = readShort();
        depth = readShort();
        int32_t length = readInt();
        compressedData = readByteArray(length);
    }
};
