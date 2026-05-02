#include "level/tile/FallingTile.hpp"

FallingTile::FallingTile(int id, int textureId) : Tile::Tile(id, textureId) {
    this->textureId = textureId;
    this->tileId = id;
}

void FallingTile::tryFall(Level* level, int x, int y, int z) {
    if (!level->isRemote) {
        int finalY = y;


        while (true) {
            int checkY = finalY-1;
            int tileId = level->getTile(x, checkY, z);
            LiquidType ltype = tileId > 0 ? Tile::tiles[tileId]->getLiquidType() : LiquidType::NOT_LIQUID;
            if (!(tileId == 0 || ltype == LiquidType::WATER || ltype == LiquidType::LAVA) || finalY <= 0) {
                if (finalY != y) {
                    tileId = level->getTile(x, finalY, z);
                    if (tileId > 0 && Tile::tiles[tileId]->getLiquidType() != LiquidType::NOT_LIQUID) {
                        level->setTileNoUpdate(x, finalY, z, 0);
                    }

                    level->swap(x, y, z, x, finalY, z);
                }

                return;
            }

            --finalY;
        }

        // while (level->getTile(x, finalY - 1, z) == 0 && finalY > 0 && (Tile::tiles[level->getTile(x, finalY-1, z)]->getLiquidType() == LiquidType::WATER || Tile::tiles[level->getTile(x, finalY-1, z)]->getLiquidType() == LiquidType::LAVA)) {
        //     finalY--;
        // }

        // if (finalY != y) {
        //     int tileId = level->getTile(x, finalY, z);
        //     if (tileId > 0 && Tile::tiles[tileId]->getLiquidType() != LiquidType::NOT_LIQUID) {
        //         level->setTileNoUpdate(x, finalY, z, 0);
        //     }
        //     level->swap(x, y, z, x, finalY, z);
        // }
    }
}

void FallingTile::onBlockAdded(Level* level, int x, int y, int z) {
    tryFall(level, x, y, z);
}

void FallingTile::neighborChanged(Level* level, int x, int y, int z, int type) {
    tryFall(level, x, y, z);
}