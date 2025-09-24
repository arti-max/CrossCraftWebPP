#pragma once
#include "Player.hpp"
#include "level/Chunk.hpp"

struct DirtyChunkSorter {
    Player* player;
    DirtyChunkSorter(Player* p) : player(p) {}
    bool operator()(Chunk* a, Chunk* b) const {
        return a->distanceToSqr(player) < b->distanceToSqr(player);
    }
};