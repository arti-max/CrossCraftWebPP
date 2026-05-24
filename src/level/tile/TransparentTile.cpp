#include "level/tile/TransparentTile.hpp"
#include "util/Random.hpp"

TransparentTile::TransparentTile(int id, int textureId) : Tile(id) {
    this->textureId = textureId;
}

bool TransparentTile::isSolid() {
    return false;
}

bool TransparentTile::blocksLight() {
    return false;
}

int TransparentTile::getDropCount() {
    Random rnd = Random();
    if (this->id == Tile::leaves->id) {
        return rnd.nextInt(10) == 0 ? 1 : 0;
    }else {
        return 1;
    }
}

int TransparentTile::getDrop() {
    if (this->id == Tile::leaves->id) {
        return Tile::bush->id;
    }
    return this->id;
}