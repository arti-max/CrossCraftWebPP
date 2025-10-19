#include "net/packet/Packet.hpp"
#include "net/packet/LoginPacket.hpp"
#include "net/packet/PositionPacket.hpp"
#include "net/packet/BlockChangePacket.hpp"

void Packet::writeByte(uint8_t value) {
    data.push_back(value);
}

void Packet::writeShort(int16_t value) {
    data.push_back((value >> 8) & 0xFF);
    data.push_back(value & 0xFF);
}

void Packet::writeInt(int32_t value) {
    data.push_back((value >> 24) & 0xFF);
    data.push_back((value >> 16) & 0xFF);
    data.push_back((value >> 8) & 0xFF);
    data.push_back(value & 0xFF);
}

void Packet::writeFloat(float value) {
    union {
        float f;
        uint32_t i;
    } converter;
    converter.f = value;
    writeInt(static_cast<int32_t>(converter.i));
}

void Packet::writeString(const std::string& str) {
    writeShort(static_cast<int16_t>(str.length()));
    for (char c : str) {
        writeByte(static_cast<uint8_t>(c));
    }
}

void Packet::writeByteArray(const uint8_t* bytes, size_t length) {
    writeInt(static_cast<int32_t>(length));
    for (size_t i = 0; i < length; i++) {
        writeByte(bytes[i]);
    }
}

uint8_t Packet::readByte() {
    if (readPos >= data.size()) return 0;
    return data[readPos++];
}

int16_t Packet::readShort() {
    if (readPos + 1 >= data.size()) return 0;
    int16_t value = (data[readPos] << 8) | data[readPos + 1];
    readPos += 2;
    return value;
}

int32_t Packet::readInt() {
    if (readPos + 3 >= data.size()) return 0;
    int32_t value = (data[readPos] << 24) | (data[readPos + 1] << 16) | 
                    (data[readPos + 2] << 8) | data[readPos + 3];
    readPos += 4;
    return value;
}

float Packet::readFloat() {
    uint32_t intValue = static_cast<uint32_t>(readInt());
    union {
        float f;
        uint32_t i;
    } converter;
    converter.i = intValue;
    return converter.f;
}

std::string Packet::readString() {
    int16_t length = readShort();
    if (length < 0 || readPos + length > data.size()) return "";
    
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; i++) {
        result += static_cast<char>(readByte());
    }
    return result;
}

std::vector<uint8_t> Packet::readByteArray(size_t length) {
    std::vector<uint8_t> result;
    result.reserve(length);
    for (size_t i = 0; i < length; i++) {
        if (readPos >= data.size()) break;
        result.push_back(readByte());
    }
    return result;
}

std::vector<uint8_t> Packet::toBytes() const {
    std::vector<uint8_t> result;
    result.push_back(static_cast<uint8_t>(type));
    result.insert(result.end(), data.begin(), data.end());
    return result;
}

Packet* Packet::fromBytes(const uint8_t* bytes, size_t length) {
    if (length < 1) {
        return nullptr;
    }
    
    PacketType type = static_cast<PacketType>(bytes[0]);
    
    Packet* packet = nullptr;
    
    switch (type) {
        case PacketType::LOGIN:
            packet = new LoginPacket();
            break;
        
        case PacketType::POSITION_UPDATE:
            packet = new PositionPacket();
            break;
        
        case PacketType::BLOCK_CHANGE:
            packet = new BlockChangePacket();
            break;
        
        default:
            return nullptr;
    }
    
    if (packet) {
        for (size_t i = 1; i < length; i++) {
            packet->data.push_back(bytes[i]);
        }
        
        packet->deserialize();
    }
    
    return packet;
}
