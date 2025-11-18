#pragma once
#include "net/packet/Packet.hpp"

class ChatMessagePacket : public Packet {
public:
    std::string message;

    ChatMessagePacket(const std::string& msg) : Packet(PacketType::CLIENT_CHAT_MESSAGE), message(msg) {}

    ChatMessagePacket() : Packet(PacketType::SERVER_CHAT_MESSAGE) {}

    void serialize() override {
        writeString(message);
    }

    void deserialize() override {
        message = readString();
    }
};
