#include "level/tile/CalmLiquidTile.hpp"

CalmLiquidTile::CalmLiquidTile(int id, int liquidType) : LiquidTile::LiquidTile(id, liquidType) {
    this->tileId = id - 1;
    this->calmTileId = id;
}

bool CalmLiquidTile::isCalmLiquid() {
    return true;
}

void CalmLiquidTile::tick(Level* level, int x, int y, int z, Random* random) {
    // no implementation
}

void CalmLiquidTile::neighborChanged(Level* level, int x, int y, int z, int type) {
    bool hasAirHeighbor = false;
    if (level->getTile(x-1, y, z) == 0) hasAirHeighbor = true;
    if (level->getTile(x+1, y, z) == 0) hasAirHeighbor = true;
    if (level->getTile(x, y, z-1) == 0) hasAirHeighbor = true;
    if (level->getTile(x, y, z+1) == 0) hasAirHeighbor = true;
    if (level->getTile(x, y-1, z) == 0) hasAirHeighbor = true;

    if (hasAirHeighbor) {
        level->setTileNoUpdate(x, y, z, this->tileId);
    }

    if (this->liquidType == 1 && type == Tile::lava->id) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }

    if (this->liquidType == 2 && type == Tile::water->id) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }
}