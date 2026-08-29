#pragma once
#include "level/tile/BushTile.hpp"

class FlowerTile : public Bush {
public:
    FlowerTile(int id, int textureId);
    void tick(Level* level, int x, int y, int z, Random* random) override;
};