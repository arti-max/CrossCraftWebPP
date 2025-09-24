#pragma once
#include "level/tile/Tile.hpp"
#include <cmath>

class Bush : public Tile {
public:
    Bush(int id);
    void tick(Level* level, int x, int y, int z, Random* random) override;
    void render(Tessellator& t, Level* level, int layer, int x, int y, int z) override;
    AABB* getAABB(int x, int y, int z) const override;
    bool blocksLight() override;
    bool isSolid() override;
};