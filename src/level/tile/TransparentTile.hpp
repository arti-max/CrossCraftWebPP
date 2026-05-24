#pragma once
#include "level/tile/Tile.hpp"

class TransparentTile : public Tile {
public:
    TransparentTile(int id, int textureId);
    bool isSolid() override;
    bool blocksLight() override;
    int getDrop() override;
    int getDropCount() override;
};