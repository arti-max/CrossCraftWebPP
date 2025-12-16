#pragma once
#include "level/tile/Tile.hpp"

class GrassTile : public Tile {
public:
    GrassTile(int id);
    int getTexture(int face) override;
    void tick(Level* level, int x, int y, int z, Random* random) override;
};