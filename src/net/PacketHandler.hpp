#pragma once
#include "net/packet/Packet.hpp"

class Client;
class CrossCraft;

class PacketHandler {
public:
    Client* client = nullptr;
    CrossCraft* cc = nullptr;
    void handleNetworkPacket(Packet* packet);
};