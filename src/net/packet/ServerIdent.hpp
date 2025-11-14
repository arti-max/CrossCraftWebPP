#pragma once
#include "net/packet/Packet.hpp"

class ServerIdentificationPacket : public Packet {
public:
    std::string serverName;
    std::string serverMotd;
    uint8_t protocolVersion = 1;

    ServerIdentificationPacket() : Packet(PacketType::SERVER_IDENTIFICATION) {}

    void serialize() override {
        // Only for server
    }

    void deserialize() override {
        protocolVersion = readByte();
        serverName = readString();
        serverMotd = readString();
    }
};
