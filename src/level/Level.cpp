#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include "level/liquid/LiquidType.hpp"
#include "util/Utils.hpp"
#include "Entity.hpp"
#include "CrossCraft.hpp"
#include <iostream>
#include <algorithm>

Level::Level() :  random() {
    
    this->random = new Random();
    randValue = random->nextInt();
    unprocessed = 0;
}

void Level::initTransient() {
    this->lightDepths.assign(this->width * this->height, 0);
    this->calcLightDepths(0, 0, this->width, this->height);
    this->tickNextTickList.clear();



    if (this->xSpawn == 0 && this->ySpawn == 0 && this->zSpawn == 0) { 
        findSpawn(); 
    }

    if (this->emesh == nullptr) {
        this->emesh = new EntityMesh(this->width, this->height, this->depth);
    }
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

float Level::getGroundLevel() const {
    return (float)(this->depth / 2 - 2);
}

float Level::getWaterLevel() const {
    return (float)(this->depth / 2);
}


void Level::findSpawn() {
    Random* rnd = new Random();
    int i = 0;

    int x;
    int y;
    int z;
    do {
        ++i;
        x = rnd->nextInt(this->width / 2) + this->width / 4;
        z = rnd->nextInt(this->height / 2) + this->height / 4;
        y = this->getHighestTile(x, y) + 1;
        if (i == 1000) {
            this->xSpawn = x;
            this->ySpawn = -100;
            this->zSpawn = z;
            return;
        }

    } while((float)y <= this->getGroundLevel());

    this->xSpawn = x;
    this->ySpawn = y;
    this->zSpawn = z;
}

int Level::getHighestTile(int x, int z) {
    int y;
    for (y = this->depth; (this->getTile(x, y -1, z) == 0 || Tile::tiles[this->getTile(x, y-1, z)]->getLiquidType() != LiquidType::NOT_LIQUID) && y > 0; --y) {
    }
    return y;
}

void Level::setSpawnPos(int x, int y, int z, int rot) {
    this->xSpawn = x;
    this->ySpawn = y;
    this->zSpawn = z;
    this->rotSpawn = rot;
}

void Level::setData(int w, int d, int h, const std::vector<uint8_t>& newBlocks) {
    width = w;
    height = h;
    depth = d;
    blocks = newBlocks;
    lightDepths.resize(w * h);
    calcLightDepths(0, 0, w, h);
    
    if (this->emesh != nullptr) {
        delete this->emesh;
    }
    this->emesh = new EntityMesh(w, h, d);

    for (LevelListener* listener : levelListeners) {
        listener->allChanged();
    }

    this->tickNextTickList.clear();
    this->findSpawn();
}

void Level::calcLightDepths(int x0, int z0, int x1, int z1) {
    for (int x = x0; x < x0 + x1; ++x) {
        for (int z = z0; z < z0 + z1; ++z) {
            
            int oldDepth = lightDepths[x + z * this->width];
            
            int depth = this->depth - 1;
            while (depth > 0 && !this->isLightBlocker(x, depth, z)) {
                --depth;
            }
            
            lightDepths[x + z * width] = depth + 1;
            
            if (oldDepth != depth) {
                int yl0 = oldDepth < depth ? oldDepth : depth;
                int yl1 = oldDepth > depth ? oldDepth : depth;
                
                for (LevelListener* listener : levelListeners) {
                    listener->lightColumnChanged(x, z, yl0, yl1);
                }
            }
        }
    }
}

void Level::tickEntities() {
    for (int i = 0; i < this->emesh->all.size(); ++i) {
        if (this->emesh->all[i]) {
            Entity* e = this->emesh->all[i];
            e->tick();

            if (e->removed) {
                utils::remove_at(this->emesh->all, i);
                i--;
                this->emesh->slotStart->init(e->xo, e->yo, e->zo).remove(e);
            } else {
                int oldX = (int)(e->xo / 16.0f);
                int oldY = (int)(e->yo / 16.0f);
                int oldZ = (int)(e->zo / 16.0f);
                int X = (int)(e->x / 16.0f);
                int Y = (int)(e->y / 16.0f);
                int Z = (int)(e->z / 16.0f);
                if (oldX != X || oldY != Y || oldZ != Z) {
                    EntityMeshSlot& s1 = this->emesh->slotStart->init(e->xo, e->yo, e->zo);
                    EntityMeshSlot& s2 = this->emesh->slotEnd->init(e->x, e->y, e->z);
                    if (s1 != s2) {
                        s1.remove(e);
                        s2.add(e);
                        e->xo = e->x;
                        e->yo = e->y;
                        e->zo = e->z;
                    }

                }

            }
        }
    }
}

void Level::tick() {
    tickCount++;

    // for (int i = 0; i < entities.size(); ++i) {
    //     entities[i]->tick();
    //     if (entities[i]->removed) {
    //         delete entities[i];
    //         entities.erase(entities.begin() + i);
    //         i--;
    //     }
    // }

    this->tickEntities();
    for (Entity* e : pendingAdd) {
        this->emesh->addEntity(e);
    }
    pendingAdd.clear();

    if (!isRemote) {
        if (tickCount % 5 == 0) {
            int count = tickNextTickList.size();
            if (count > 1000) count = 1000;

            for (int i = 0; i < count; ++i) {
                TickEntry entry = tickNextTickList.front();
                tickNextTickList.pop_front();

                if (getTile(entry.x, entry.y, entry.z) == entry.tileId) {
                    Tile::tiles[entry.tileId]->tick(this, entry.x, entry.y, entry.z, random);
                }
            }
        }

    
        unprocessed += width * height * depth;
        int ticks = unprocessed / TILE_UPDATE_INTERVAL;
        unprocessed -= ticks * TILE_UPDATE_INTERVAL;
        
        for (int i = 0; i < ticks; ++i) {
            int x = random->nextInt(width);
            int y = random->nextInt(depth);
            int z = random->nextInt(height);
            
            int id = getTile(x, y, z);
            if (id != 0) {
                Tile* tile = Tile::tiles[id];
                if (tile) {
                    tile->tick(this, x, y, z, random);
                }
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
    return this->isLit(x, y, z) ? 1.0f : 0.5f;
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
    if (x < 0 || y < 0 || z < 0 || x >= width || y >= depth || z >= height) {
        return false;
    }

    if (type == 0 && !isRemote) {
        if (this->isBanned(x, y, z)) {
            this->removeBanned(x, y, z, type);
        }
        if (x == 0 || z == 0 || x == width - 1 || z == height - 1) {
            if (y >= this->getGroundLevel() && y < this->getWaterLevel()) {
                type = Tile::water->id;
            }
        }
    }

    if ((type == Tile::water->id || type == Tile::calmWater->id) && !isRemote) {
        for (int xx = x - 2; xx <= x + 2; xx++) {
            for (int yy = y - 2; yy <= y + 2; yy++) {
                for (int zz = z - 2; zz <= z + 2; zz++) {
                    if (this->getTile(xx, yy, zz) == 19 && (x > 0 && z > 0 && x < width && z < height)) {
                        type = 0;
                    }
                }
            }
        }
    }
        
    int index = (y * height + z) * width + x;
    int oldType = blocks[index];
    
    if (type == oldType) {
        return false;
    }
    
    blocks[index] = static_cast<uint8_t>(type);
    
    if (type > 0 && Tile::tiles[type] != nullptr) {
        Tile::tiles[type]->onBlockAdded(this, x, y, z);
    }
    
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

void Level::swap(int x1, int y1, int z1, int x2, int y2, int z2) {
    if (!this->isRemote) {
        int tile1 = getTile(x1, y1, z1);
        int tile2 = getTile(x2, y2, z2);

        setTileNoUpdate(x1, y1, z1, tile2);
        setTileNoUpdate(x2, y2, z2, tile1);

        neighborChanged(x1 - 1, y1, z1, tile2);
        neighborChanged(x1 + 1, y1, z1, tile2);
        neighborChanged(x1, y1 - 1, z1, tile2);
        neighborChanged(x1, y1 + 1, z1, tile2);
        neighborChanged(x1, y1, z1 - 1, tile2);
        neighborChanged(x1, y1, z1 + 1, tile2);

        neighborChanged(x2 - 1, y2, z2, tile1);
        neighborChanged(x2 + 1, y2, z2, tile1);
        neighborChanged(x2, y2 - 1, z2, tile1);
        neighborChanged(x2, y2 + 1, z2, tile1);
        neighborChanged(x2, y2, z2 - 1, tile1);
        neighborChanged(x2, y2, z2 + 1, tile1);
    }
}

std::vector<AABB> Level::getCubes(const AABB& boundingBox) {
    std::vector<AABB> boxes;
    
    int x0 = (int)(boundingBox.x0);
    int x1 = (int)(boundingBox.x1) + 1;
    int y0 = (int)(boundingBox.y0);
    int y1 = (int)(boundingBox.y1) + 1;
    int z0 = (int)(boundingBox.z0);
    int z1 = (int)(boundingBox.z1) + 1;
    if (boundingBox.x0 < 0.0f) --x0;
    if (boundingBox.y0 < 0.0f) --y0;
    if (boundingBox.z0 < 0.0f) --z0;
    
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
    int x0 = (int)(box.x0);
    int x1 = (int)(box.x1 + 1.0f);
    int y0 = (int)(box.y0);
    int y1 = (int)(box.y1 + 1.0f);
    int z0 = (int)(box.z0);
    int z1 = (int)(box.z1 + 1.0f);
    if (box.x0 < 0.0f) --x0;
    if (box.y0 < 0.0f) --y0;
    if (box.z0 < 0.0f) --z0;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 > this->width)   x1 = this->width;
    if (y1 > this->depth)   y1 = this->depth;
    if (z1 > this->height)  z1 = this->height;
    
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                Tile* tile = Tile::tiles[getTile(x, y, z)];
                if (tile != nullptr && tile->getLiquidType() != LiquidType::NOT_LIQUID) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool Level::containsLiquid(const AABB& box, LiquidType liquidType) {
    int x0 = (int)std::floor(box.x0);
    int x1 = (int)std::floor(box.x1 + 1.0f);
    int y0 = (int)std::floor(box.y0);
    int y1 = (int)std::floor(box.y1 + 1.0f);
    int z0 = (int)std::floor(box.z0);
    int z1 = (int)std::floor(box.z1 + 1.0f);
    if (box.x0 < 0.0f) --x0;
    if (box.y0 < 0.0f) --y0;
    if (box.z0 < 0.0f) --z0;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 > this->width)   x1 = this->width;
    if (y1 > this->depth)   y1 = this->depth;
    if (z1 > this->height)  z1 = this->height;
    
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                Tile* tile = Tile::tiles[getTile(x, y, z)];
                if (tile != nullptr && tile->getLiquidType() == liquidType) {
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

void Level::addTick(int x, int y, int z) {
    ticking.insert(encodePosition(x, y, z));
}

void Level::removeTick(int x, int y, int z) {
    ticking.erase(encodePosition(x, y, z));
}

void Level::addToTickNextTick(int x, int y, int z, int tileId) {
    this->tickNextTickList.push_back({x, y, z, tileId});
}

bool Level::needsTick(int tileId) {
    return 
        tileId == Tile::water->id   ||
        tileId == Tile::lava->id
        ;
}

void Level::addBanned(int x, int y, int z, int id) {
    this->bannedTiles.push_back({x, y, z, id});
}

void Level::removeBanned(int x, int y, int z, int id) {
    auto it = this->bannedTiles.begin();
    while (it != this->bannedTiles.end()) {
        if (it->x == x && it->y == y && it->z == z && it->tileId == id) {
            it = this->bannedTiles.erase(it);
            return; 
        } else {
            ++it;
        }
    }
}

bool Level::isBanned(int x, int y, int z) {
    for (TickEntry tile : this->bannedTiles) {
        if (tile.x == x && tile.y == y && tile.z == z) {
            return true;
        }
    }
    return false;
}

void Level::playSound(const std::string& name, Entity* entity, float volume, float pitch) {
    CrossCraft* cc = this->cc;
    if (this->cc != nullptr && cc->sound != nullptr) {
        cc->sound->playAt(name, entity->x, entity->y, entity->z, pitch, volume);
    }
}

void Level::playSound(const std::string& name, float x, float y, float z, float volume, float pitch) {
    CrossCraft* cc = this->cc;
    if (this->cc != nullptr && cc->sound != nullptr) {
        cc->sound->playAt(name, x, y, z, pitch, volume);
    }
}

void Level::playSound(const std::string& name, float volume, float pitch) {
    CrossCraft* cc = this->cc;
    if (this->cc != nullptr && cc->sound != nullptr) {
        cc->sound->playCentered(name, pitch, volume);
    }
}

void Level::addEntity(Entity* e) {
    pendingAdd.push_back(e);
    e->setLevel(this);
}

void Level::removeEntity(Entity* e) {
    this->emesh->removeEntity(e);
}

std::vector<Entity*> Level::findEntities(Entity* ignore, const AABB& bbox) {
    return this->emesh->getEntities(ignore, bbox);
}

Entity* Level::getPlayer() {
    return this->player;
}