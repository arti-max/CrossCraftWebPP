#include "net/NetworkData.hpp"
#include "CrossCraft.hpp"
#include "HitResult.hpp"
#include "net/NetworkPlayer.hpp"
#include "net/PacketHandler.hpp"
#include "level/Level.hpp"
#include "Entity.hpp"

NetworkData::NetworkData(CrossCraft* cc) {
    this->cc = cc;
    this->handler = new PacketHandler();
    this->handler->cc = cc;
}


void NetworkData::addEntity(Entity* e, int id) {
    auto it = this->entities.find(id);
    if (it != this->entities.end()) {
        this->removeEntity(id);
    }
    e->setNetwork(true);
    this->entities[id] = e;
    this->cc->level->addEntity(e);
}

void NetworkData::removeEntity(int id) {
    auto it = this->entities.find(id);
    if (it != this->entities.end()) {
        Entity* e = it->second;
        this->cc->level->removeEntity(e);
    }
    this->entities.erase(it);
}

void NetworkData::addplayer(NetworkPlayer* player, int id) {
    auto it = this->players.find(id);
    player->setNetwork(true);
    if (it == this->players.end()) {
        this->players[id] = player;
        this->cc->level->addEntity(player);
    } else {
        delete it->second;
        it->second = player;
    }
}

void NetworkData::removePlayer(int id) {
    auto it = this->players.find(id);
    if (it != this->players.end()) {
        this->cc->level->removeEntity(it->second);
        this->players.erase(it);
    }
}

void NetworkData::tick() {
    for (auto const& [id, net_player] : this->players) {
        if (net_player != nullptr) {
            net_player->tick();
        }
    }
}

void NetworkData::render(float partialTicks) {
    for (auto const& [id, net_player] : this->players) {
        if (net_player != nullptr) {
            net_player->render(this->cc->textures, partialTicks, this->cc->font, this->cc->player);
        }
    }
}

int NetworkData::addHit(HitResult* hit, int tileId) {
    static int nextHitId = 1;
    int hitId = nextHitId++;

    this->hits[hitId] = {hit, tileId, 0};
    return hitId;
}

HitInfo* NetworkData::getHit(int hitId) {
    auto it = this->hits.find(hitId);
    if (it != this->hits.end()) {
        return &it->second;
    }
    return nullptr;
}

void NetworkData::stopHit(int hitId) {
    auto it = this->hits.find(hitId);
    if (it != this->hits.end()) {
        if (it->second.hit != nullptr) {
            delete it->second.hit;
        }
        this->hits.erase(it);
    }
}

void NetworkData::setPlayerPosition(float x, float y, float z, float yaw, float pitch, int id) {
    auto it = this->players.find(id);
    if (it != this->players.end()) {
        it->second->queue(x, y, z, yaw, pitch);
    }
}