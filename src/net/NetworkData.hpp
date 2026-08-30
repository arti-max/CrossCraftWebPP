#pragma once
#include "net/NetworkPlayer.hpp"
#include "net/PacketHandler.hpp"
#include <map>
#include <vector>

class CrossCraft;
class HitResult;
class Entity;

struct HitInfo {
    HitResult* hit = nullptr;
    int tileId = 0;
    int hits = 0;
};

class NetworkData {
private:
    std::map<int, HitInfo> hits;
    std::map<int, Entity*> entities;
    std::map<int, NetworkPlayer*> players;
    CrossCraft* cc = nullptr;
public:
    PacketHandler* handler = nullptr;
    NetworkData(CrossCraft* cc);
    int addHit(HitResult* hit, int tileId); // return: hit id
    void addplayer(NetworkPlayer* player, int id);
    void removePlayer(int id);
    NetworkPlayer* getplayer(int id);
    void addEntity(Entity* e, int id);
    void removeEntity(int id);
    Entity* getEntity(int id);
    void stopHit(int hitId);
    HitInfo* getHit(int hitId);

    void tick();
    void render();
};