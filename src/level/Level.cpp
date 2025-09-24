#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include <iostream>
#include <algorithm>

Level::Level(int width, int height, int depth) 
    : width(width), height(height), depth(depth), random() {
    
    blocks.resize(width * height * depth, 0);
    lightDepths.resize(width * height, 0);
    this->random = new Random();
    randValue = random->nextInt();
    unprocessed = 0;
    
    generateMap();
    
    calcLightDepths(0, 0, width, height);
    
    std::cout << "Level created: " << width << "x" << height << "x" << depth << std::endl;
}

void Level::generateMap() {
    std::fill(blocks.begin(), blocks.end(), 0);
    
    int groundLevel = depth * 2 / 3;
    
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < height; ++z) {
            for (int y = 0; y < depth; ++y) {
                int index = (y * height + z) * width + x;
                
                if (index >= 0 && index < static_cast<int>(blocks.size())) {
                    if (y < groundLevel - 1) {
                        blocks[index] = Tile::rock->id;
                    } else if (y == groundLevel - 1) {
                        blocks[index] = Tile::grass->id;
                    }
                } else {
                    std::cout << "ERROR: Index out of bounds: " << index << " (size: " << blocks.size() << ")" << std::endl;
                }
            }
        }
    }
    
    // generateCaves();

    std::cout << "Generated simple world with ground level at " << groundLevel << std::endl;
}

void Level::generateCaves() {
    std::cout << "Generating caves..." << std::endl;
    
    // Generate 10000 caves 
    for (int i = 0; i < 10000; i++) {
        int caveSize = random->nextInt(7) + 1;
        
        int caveX = random->nextInt(width);
        int caveY = random->nextInt(depth);  // depth соответствует Y в Java
        int caveZ = random->nextInt(height); // height соответствует Z в Java
        
        // Grow cave
        for (int radius = 0; radius < caveSize; radius++) {
            for (int sphere = 0; sphere < 1000; sphere++) {
                int offsetX = random->nextInt(radius * 2 + 1) - radius;
                int offsetY = random->nextInt(radius * 2 + 1) - radius;  
                int offsetZ = random->nextInt(radius * 2 + 1) - radius;
                
                // Sphere shape - оптимизация: избегаем std::pow для квадратов
                int distanceSquared = offsetX * offsetX + offsetY * offsetY + offsetZ * offsetZ;
                int radiusSquared = radius * radius;
                
                if (distanceSquared > radiusSquared) {
                    continue;
                }
                
                int tileX = caveX + offsetX;
                int tileY = caveY + offsetY;
                int tileZ = caveZ + offsetZ;
                
                // Calculate index from x, y and z
                int index = (tileY * height + tileZ) * width + tileX;
                
                // Check if tile is out of level
                if (index >= 0 && index < static_cast<int>(blocks.size())) {
                    // Border of level
                    if (tileX > 0 && tileY > 0 && tileZ > 0 &&
                        tileX < width - 1 && tileY < depth - 1 && tileZ < height - 1) {
                        
                        // Fill level with air (0 = empty)
                        blocks[index] = 0;
                    }
                }
            }
        }
    }
    
    std::cout << "Cave generation completed!" << std::endl;
}

void Level::setData(int w, int d, int h, const std::vector<uint8_t>& newBlocks) {
    width = w;
    height = h;
    depth = d;
    blocks = newBlocks;
    lightDepths.resize(w * h);
    calcLightDepths(0, 0, w, h);
    
    for (LevelListener* listener : levelListeners) {
        listener->allChanged();
    }
}

void Level::calcLightDepths(int x0, int z0, int x1, int z1) {
    for (int x = x0; x < x0 + x1; ++x) {
        for (int z = z0; z < z0 + z1; ++z) {
            
            int oldDepth = lightDepths[x + z * this->width];
            
            int depth = this->depth - 1;
            while (depth > 0 && !this->isLightBlocker(x, depth, z)) {
                --depth;
            }
            
            lightDepths[x + z * width] = depth;
            
            if (oldDepth != depth) {
                int yl0 = std::min(oldDepth, depth);
                int yl1 = std::max(oldDepth, depth);
                
                for (LevelListener* listener : levelListeners) {
                    listener->lightColumnChanged(x, z, yl0, yl1);
                }
            }
        }
    }
}

void Level::tick() {
    unprocessed += width * height * depth;
    int ticks = unprocessed / TILE_UPDATE_INTERVAL;
    unprocessed -= ticks * TILE_UPDATE_INTERVAL;
    
    tickLiquids();
    
    for (int i = 0; i < ticks; ++i) {
        randValue = randValue * multiplier + addend;
        int x = (randValue >> 16) & (width - 1);
        randValue = randValue * multiplier + addend;
        int y = (randValue >> 16) & (depth - 1);
        randValue = randValue * multiplier + addend;
        int z = (randValue >> 16) & (height - 1);
        
        int id = getTile(x, y, z);
        if (id != 0) {
            Tile* tile = Tile::tiles[id];
            if (tile) {
                tile->tick(this, x, y, z, random);
            }
        }
    }
}

bool Level::isTile(int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= width || y >= depth || z >= height) {
        return false;
    }
    int index = (y * height + z) * width + x;
    return blocks[index] != 0;
}

bool Level::isSolidTile(int x, int y, int z) {
    Tile* tile = Tile::tiles[getTile(x, y, z)];
    return tile != nullptr && tile->isSolid();
}

bool Level::isLightBlocker(int x, int y, int z) {
    Tile* tile = Tile::tiles[getTile(x, y, z)];
    return tile != nullptr && tile->blocksLight();
}

float Level::getBrightness(int x, int y, int z) {
    float dark = 0.0f;
    float light = 1.0f;
    
    if (x < 0 || y < 0 || z < 0 || x >= width || y >= depth || z >= height) {
        return light;
    }
    
    if (y < lightDepths[x + z * width]) {
        return dark;
    }
    
    return light;
}

bool Level::isLit(int x, int y, int z) {
    if (x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
        return y >= lightDepths[x + z * width];
    }
    return true;
}

int Level::getTile(int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= width || y >= depth || z >= height) {
        return 0;
    }
    int index = (y * height + z) * width + x;
    return blocks[index];
}

bool Level::setTile(int x, int y, int z, int type) {
    if (x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
        int index = (y * height + z) * width + x;
        int oldType = blocks[index];
        
        if (type == oldType) {
            return false;
        }
        
        if (isActiveLiquidTile(oldType)) {
            removeLiquidPosition(x, y, z);
        }
        if (isActiveLiquidTile(type)) {
            addLiquidPosition(x, y, z);
        }
        
        blocks[index] = static_cast<uint8_t>(type);
        
        neighborChanged(x - 1, y, z, type);
        neighborChanged(x + 1, y, z, type);
        neighborChanged(x, y - 1, z, type);
        neighborChanged(x, y + 1, z, type);
        neighborChanged(x, y, z - 1, type);
        neighborChanged(x, y, z + 1, type);
        
        calcLightDepths(x, z, 1, 1);
        
        for (LevelListener* listener : levelListeners) {
            listener->tileChanged(x, y, z);
        }
        
        return true;
    }
    return false;
}

bool Level::setTileNoUpdate(int x, int y, int z, int type) {
    if (x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
        int index = (y * height + z) * width + x;
        if (type == blocks[index]) {
            return false;
        }
        blocks[index] = static_cast<uint8_t>(type);
        return true;
    }
    return false;
}

std::vector<AABB> Level::getCubes(const AABB& boundingBox) {
    std::vector<AABB> boxes;
    
    int x0 = static_cast<int>(std::floor(boundingBox.x0)) - 1;
    int x1 = static_cast<int>(std::ceil(boundingBox.x1)) + 1;
    int y0 = static_cast<int>(std::floor(boundingBox.y0)) - 1;
    int y1 = static_cast<int>(std::ceil(boundingBox.y1)) + 1;
    int z0 = static_cast<int>(std::floor(boundingBox.z0)) - 1;
    int z1 = static_cast<int>(std::ceil(boundingBox.z1)) + 1;
    
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                if (x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
                    Tile* tile = Tile::tiles[getTile(x, y, z)];
                    if (tile != nullptr) {
                        AABB* aabb = tile->getAABB(x, y, z);
                        if (aabb != nullptr) {
                            boxes.push_back(*aabb);
                            delete aabb;
                        }
                    }
                } else if (x < 0 || y < 0 || z < 0 || x >= width || z >= height) {
                    AABB* aabb = Tile::unbreakable->getAABB(x, y, z);
                    if (aabb != nullptr) {
                        boxes.push_back(*aabb);
                        delete aabb;
                    }
                }
            }
        }
    }
    
    return boxes;
}

bool Level::containsAnyLiquid(const AABB& box) {
    int x0 = std::max(0, static_cast<int>(std::floor(box.x0)));
    int x1 = std::min(width, static_cast<int>(std::floor(box.x1 + 1.0f)));
    int y0 = std::max(0, static_cast<int>(std::floor(box.y0)));
    int y1 = std::min(depth, static_cast<int>(std::floor(box.y1 + 1.0f)));
    int z0 = std::max(0, static_cast<int>(std::floor(box.z0)));
    int z1 = std::min(height, static_cast<int>(std::floor(box.z1 + 1.0f)));
    
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                Tile* tile = Tile::tiles[getTile(x, y, z)];
                if (tile != nullptr && tile->getLiquidType() > 0) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool Level::containsLiquid(const AABB& box, int liquidId) {
    int x0 = std::max(0, static_cast<int>(std::floor(box.x0)));
    int x1 = std::min(width, static_cast<int>(std::floor(box.x1 + 1.0f)));
    int y0 = std::max(0, static_cast<int>(std::floor(box.y0)));
    int y1 = std::min(depth, static_cast<int>(std::floor(box.y1 + 1.0f)));
    int z0 = std::max(0, static_cast<int>(std::floor(box.z0)));
    int z1 = std::min(height, static_cast<int>(std::floor(box.z1 + 1.0f)));
    
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                Tile* tile = Tile::tiles[getTile(x, y, z)];
                if (tile != nullptr && tile->getLiquidType() == liquidId) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void Level::addListener(LevelListener* listener) {
    levelListeners.push_back(listener);
}

void Level::removeListener(LevelListener* listener) {
    auto it = std::find(levelListeners.begin(), levelListeners.end(), listener);
    if (it != levelListeners.end()) {
        levelListeners.erase(it);
    }
}

void Level::neighborChanged(int x, int y, int z, int type) {
    if (x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
        Tile* tile = Tile::tiles[getTile(x, y, z)];
        if (tile != nullptr) {
            tile->neighborChanged(this, x, y, z, type);
        }
    }
}

void Level::tickLiquids() {
    std::vector<int> positionsToUpdate(liquidPositions.begin(), liquidPositions.end());
    
    for (int positionCode : positionsToUpdate) {
        int x, y, z;
        decodePosition(positionCode, x, y, z);
        
        int tileId = getTile(x, y, z);
        Tile* tile = Tile::tiles[tileId];
        
        if (tile != nullptr) {
            if (tile->isCalmLiquid()) {
                removeLiquidPosition(x, y, z);
                continue;
            }
            tile->tick(this, x, y, z, random);
        }
    }
}

bool Level::isLiquidTile(int tileId) {
    return 
        tileId == Tile::water->id || 
        tileId == Tile::calmWater->id || 
        tileId == Tile::lava->id || 
        tileId == Tile::calmLava->id;
}

bool Level::isActiveLiquidTile(int tileId) {
    return tileId == Tile::water->id || tileId == Tile::lava->id;
}

int Level::encodePosition(int x, int y, int z) {
    return (z << (maxBits * 2)) | (y << maxBits) | x;
}

void Level::decodePosition(int code, int& x, int& y, int& z) {
    int mask = (1 << maxBits) - 1;
    x = code & mask;
    y = (code >> maxBits) & mask;
    z = (code >> (maxBits * 2)) & mask;
}

void Level::addLiquidPosition(int x, int y, int z) {
    liquidPositions.insert(encodePosition(x, y, z));
}

void Level::removeLiquidPosition(int x, int y, int z) {
    liquidPositions.erase(encodePosition(x, y, z));
}
