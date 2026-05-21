#include "level/tile/LogTile.hpp"
#include "util/Random.hpp"

LogTile::LogTile(int id) : Tile(id) {
    this->textureId = 20;
}

int LogTile::getTexture(int face) {
    return face == 1 ? 21 : face == 0 ? 21 : 20;
}

int LogTile::getDrop() {
    return Tile::wood->id;
}

int LogTile::getDropCount() {
    Random rnd = Random();
    return rnd.nextInt(3) + 3;
}
