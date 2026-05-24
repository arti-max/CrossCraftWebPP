#pragma once
#include "level/tile/Tile.hpp"

class CrossCraft;

class HeldBlock {
public:
    CrossCraft* cc;
    Tile* tile = nullptr;
    float pos = 0.0f;
    float lastPos = 0.0f;
    int offset = 0;
    bool moving = false;

    HeldBlock(CrossCraft* cc) {
        this->cc = cc;
    }
    
};