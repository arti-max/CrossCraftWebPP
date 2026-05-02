#pragma once
#include "level/tile/Tile.hpp"

class SpongeTile : public Tile {
public:
    SpongeTile(int id);
    void onBlockAdded(Level* level, int x, int y, int z) override;
    void neighborChanged(Level* level, int x, int y, int z, int type) override;

private:
    int x = 0, y = 0, z = 0;
    void checkSpongeEffect(Level* level, int x, int y, int z);
};