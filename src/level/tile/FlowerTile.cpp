#include "level/tile/FlowerTile.hpp"

FlowerTile::FlowerTile(int id, int textureId) : Bush(id, textureId) {
    float size = 0.2f;
    this->setShape(0.5f - size, 0.0f, 0.5f - size, size + 0.5f, size * 3.0f, size + 0.5f);
}

void FlowerTile::tick(Level* level, int x, int y, int z, Random* random) {
    if (!level->growTrees) {
        int tileIdBelow = level->getTile(x, y-1, z);
        if (!level->isLit(x, y, z) || tileIdBelow != Tile::dirt->id && tileIdBelow != Tile::grass->id) {
            level->setTile(x, y, z, 0);
        }
    }
}