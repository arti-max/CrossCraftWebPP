#include "level/tile/TNTTile.hpp"
#include "item/TNT.hpp"

TNTTile::TNTTile(int tileId) : Tile(tileId) {
    this->textureId = 8;
}

int TNTTile::getTexture(int face) {
    return face == 0 ? 10 : face == 1 ? 9 : 8;
}

void TNTTile::onDestroy(Level* level, int x, int y, int z) {
    level->addEntity(new TNT(level, x, y, z));
}

int TNTTile::getDropCount() {
    return 0;
}