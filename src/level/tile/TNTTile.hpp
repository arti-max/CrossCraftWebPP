#pragma once
#include "level/tile/Tile.hpp"

class TNTTile : public Tile {

public:
    TNTTile(int tileId);
    void onDestroy(Level* level, int x, int y, int z) override;
    int getTexture(int face) override;
    int getDropCount() override;
};