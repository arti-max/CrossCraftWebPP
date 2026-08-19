#include "level/tile/SlabTile.hpp"

SlabTile::SlabTile(int tileId, int textureId) : Tile(tileId, textureId) {
    this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
}

bool SlabTile::shouldRenderFace(Level* level, int x, int y, int z, int layer, int face) {
    if (layer == 1 ) return false;
    if (face == 1) return true;
    return Tile::shouldRenderFace(level, x, y, z, layer, face);
}