#pragma once
#include "level/tile/BushTile.hpp"

class MushroomTile : public Bush {
public:
    MushroomTile(int id, int textureId);
    void tick(Level* level, int x, int y, int z, Random* random) override;
};