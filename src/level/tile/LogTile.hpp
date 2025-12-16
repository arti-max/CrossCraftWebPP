#pragma once
#include "level/tile/Tile.hpp"

class LogTile : public Tile {
public:
    LogTile(int id);
    int getTexture(int face) override;
};