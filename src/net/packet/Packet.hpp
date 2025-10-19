#pragma once
#include "PacketType.hpp"
#include <vector>
#include <cstdint>
#include <cstring>

class Packet {
protected:
    PacketType type;
    std::vector<uint8_t> data;
    size_t readPos = 0;

public:
    Packet(PacketType type) : type(type) {}
    virtual ~Packet() = default;

    PacketType getType() const { return type; }
    const std::vector<uint8_t>& getData() const { return data; }
    
    void writeByte(uint8_t value);
    void writeShort(int16_t value);
    void writeInt(int32_t value);
    void writeFloat(float value);
    void writeString(const std::string& str);
    void writeByteArray(const uint8_t* bytes, size_t length);
    
    uint8_t readByte();
    int16_t readShort();
    int32_t readInt();
    float readFloat();
    std::string readString();
    std::vector<uint8_t> readByteArray(size_t length);
    
    void resetRead() { readPos = 0; }
    
    virtual void serialize() = 0;
    virtual void deserialize() = 0;
    
    std::vector<uint8_t> toBytes() const;
    
    static Packet* fromBytes(const uint8_t* bytes, size_t length);
};
