#include "level/tile/LiquidTile.hpp"

LiquidTile::LiquidTile(int id, int liquidType) : Tile::Tile(id) {
    this->liquidType = liquidType;
    if (this->liquidType == 2) {
        this->textureId = 30;
    } else {
        this->textureId = 14;
    }

    if (this->liquidType == 1) {
        this->spreadSpeed = 8;
    } else {
        this->spreadSpeed = 2;
    }

    this->tileId = id;
    this->calmTileId = id++;
    float dd = 0.1f;
    this->setShape(0.0f, 0.0f - dd, 0.0f, 1.0f, 1.0f - dd, 1.0f);
}

void LiquidTile::tick(Level* level, int x, int y, int z, Random* random) {
    this->updateWater(level, x, y, z, 0);
}

bool LiquidTile::updateWater(Level* level, int x, int y, int z, int depth) {
    bool hasChanged = false;

    bool change;
    do {
        --y;
        if (level->getTile(x, y, z) != 0) {
            break;
        }
        change = level->setTile(x, y, z, this->tileId);
        if (change) {
            hasChanged = true;
        }
    } while (change && this->liquidType != 2);

    ++y;
    if (this->liquidType == 1 || !hasChanged) {
        hasChanged |= this->checkWater(level, x - 1, y, z, depth);
        hasChanged |= this->checkWater(level, x + 1, y, z, depth);
        hasChanged |= this->checkWater(level, x, y, z - 1, depth);
        hasChanged |= this->checkWater(level, x, y, z + 1, depth);
    }

    if (!hasChanged) {
        level->setTileNoUpdate(x, y, z, this->calmTileId);
    }

    return hasChanged;
}

bool LiquidTile::checkWater(Level* level, int x, int y, int z, int depth) {
    bool hasChanged = false;
    int type = level->getTile(x, y, z);
    if (type == 0) {
        bool changed = level->setTile(x, y, z, this->tileId);
        if (changed && depth < this->spreadSpeed) {
            hasChanged |= this->updateWater(level, x, y, z, depth+1);
        }
    }

    return hasChanged;
}

bool LiquidTile::shouldRenderFace(Level* level, int x, int y, int z, int layer, int face) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth) {
        return false;
    }
    if (layer != 2 && this->liquidType == 1) {
        return false;
    }
    
    int neighborId = level->getTile(x, y, z);
    
    if (neighborId == this->tileId || neighborId == this->calmTileId) {
        return false;
    }
    
    if (level->isSolidTile(x, y, z)) {
        return false;
    }
    
    return true;
}

void LiquidTile::renderFace(Tessellator& t, int x, int y, int z, int face) {
    Tile::renderFace(t, x, y, z, face);
    
    if (face == 0 || face == 1) {
        Tile::renderBackFace(t, x, y, z, face);
    }
}

bool LiquidTile::mayPick() {
    return false;
}

AABB* LiquidTile::getAABB(int x, int y, int z) const {
    return nullptr;
}

bool LiquidTile::blocksLight() {
    return true;
}

bool LiquidTile::isSolid() {
    return false;
}

int LiquidTile::getLiquidType() {
    return this->liquidType;
}

void LiquidTile::neighborChanged(Level* level, int x, int y, int z, int type) {
    if (this->liquidType == 1 && (type == Tile::lava->id || type == Tile::calmLava->id)) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }
    if (this->liquidType == 2 && (type == Tile::water->id || type == Tile::calmWater->id)) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }
}