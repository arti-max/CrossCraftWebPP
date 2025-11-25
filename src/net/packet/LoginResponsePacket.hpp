#pragma once
#include "net/packet/Packet.hpp"

class LoginResponsePacket : public Packet {
public:
    std::string username;

    LoginResponsePacket() : Packet(PacketType::LOGIN_RESPONSE) {
        type = PacketType::LOGIN_RESPONSE;
    }
    
    LoginResponsePacket(std::string name) : Packet(PacketType::LOGIN_RESPONSE) {
        type = PacketType::LOGIN_RESPONSE;
        username = name;
    }

    void serialize() override {
        writeString(username);
    }

    void deserialize() override {
        username = readString();
    }
};
