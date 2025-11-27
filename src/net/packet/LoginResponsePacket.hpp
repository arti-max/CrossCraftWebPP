#pragma once
#include "net/packet/Packet.hpp"

class LoginResponsePacket : public Packet {
public:
    std::string username;
    int8_t playerId;

    LoginResponsePacket() : Packet(PacketType::LOGIN_RESPONSE) {
        type = PacketType::LOGIN_RESPONSE;
    }
    
    LoginResponsePacket(std::string name) : Packet(PacketType::LOGIN_RESPONSE) {
        type = PacketType::LOGIN_RESPONSE;
        username = name;
    }

    void serialize() override {
        writeByte(playerId);
        writeString(username);
    }

    void deserialize() override {
        playerId = readByte();
        username = readString();
    }
};
