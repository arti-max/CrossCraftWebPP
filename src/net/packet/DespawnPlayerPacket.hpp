#pragma once
#include "net/packet/Packet.hpp"

class DespawnPlayerPacket : public Packet {
public:
    int8_t playerId;

    DespawnPlayerPacket() : Packet(PacketType::PLAYER_DESPAWN) {}

    void serialize() override {
        // Only for server
    }

    void deserialize() override {
        playerId = readByte();
    }
};
