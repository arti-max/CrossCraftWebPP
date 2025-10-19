#pragma once
#include "net/packet/Packet.hpp"

class LoginPacket : public Packet {
public:
    std::string username;
    std::string sessionId;
    uint8_t protocolVersion = 1;

    LoginPacket() : Packet(PacketType::LOGIN) {}
    
    LoginPacket(const std::string& username, const std::string& sessionId)
        : Packet(PacketType::LOGIN), username(username), sessionId(sessionId) {}

    void serialize() override {
        writeByte(protocolVersion);
        writeString(username);
        writeString(sessionId);
    }

    void deserialize() override {
        protocolVersion = readByte();
        username = readString();
        sessionId = readString();
    }
};
