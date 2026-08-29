#include "level/tile/OreTile.hpp"
#include "util/Random.hpp"

OreTile::OreTile(int id, int textureId) : Tile(id) {
    this->textureId = textureId;
}

int OreTile::getDrop() {
    return this->id == Tile::coalOre->id ? Tile::slab->id : this->id == Tile::ironOre->id ? Tile::ironBlock->id : this->id == Tile::goldOre->id ? Tile::goldBlock->id : 0;
}

int OreTile::getDropCount() {
    Random rnd;
    return rnd.nextInt(3) + 1; // 1 - 4
}