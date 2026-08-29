#pragma once
#include "level/tile/Tile.hpp"

class BookshelfTile : public Tile {
public:
    BookshelfTile(int id);
    int getTexture(int face) override;
    int getDropCount() override;
};