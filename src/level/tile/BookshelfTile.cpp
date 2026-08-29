#include "level/tile/BookshelfTile.hpp"

BookshelfTile::BookshelfTile(int id) : Tile(id) {
    this->textureId = 35;
}

int BookshelfTile::getTexture(int face) {
    return face <= 1 ? 4 : 35;
}

int BookshelfTile::getDropCount() {
    return 0;
}