#include "level/tile/MushroomTile.hpp"

MushroomTile::MushroomTile(int id, int textureId) : Bush(id, textureId) {
    float size = 0.2f;
    this->setShape(0.5f - size, 0.0f, 0.5f - size, size + 0.5f, size * 2.0f, size + 0.5f);
}

void MushroomTile::tick(Level* level, int x, int y, int z, Random* random) {
    int tileIdBelow = level->getTile(x, y-1, z);
    if (level->isLit(x, y, z) || tileIdBelow != Tile::rock->id && tileIdBelow != Tile::gravel->id && tileIdBelow != Tile::cobblestone->id) {
        level->setTile(x, y, z, 0);
    }
}