#pragma once
#include "level/tile/Tile.hpp"

class SlabTile : public Tile {
protected:
    bool shouldRenderFace(Level* level, int x, int y, int z, int layer, int face) override;
public:
    SlabTile(int tileId, int textureId);

};