#pragma once
#include "level/tile/Tile.hpp"

class OreTile : public Tile {
public:
    OreTile(int id, int textureId);
    int getDrop() override;
    int getDropCount() override;
};