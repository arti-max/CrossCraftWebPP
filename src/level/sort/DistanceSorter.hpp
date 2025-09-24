#pragma once
#include "Player.hpp"
#include "level/Chunk.hpp"

struct DistanceSorter {
    Player* player;
    DistanceSorter(Player* p) : player(p) {}
    bool operator()(Chunk* a, Chunk* b) const {
        return a->distanceToSqr(player) < b->distanceToSqr(player);
    }
};