#include "LevelGen.hpp"
#include "level/tile/Tile.hpp"
#include <cmath>
#include <algorithm>

LevelGen::LevelGen(LevelLoaderListener* listener) : levelLoaderListener(listener) {
}

LevelGen::~LevelGen() {
    delete random;
}

void LevelGen::generateLevel(Level* level, const char* username, int width, int height, int depth) {
    this->levelLoaderListener->beginLevelLoading("Generating level");
    this->levelLoaderListener->levelLoadUpdate("Raising...");
    
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->blocks.resize(width * height * depth, 0);
    
    this->generateMap();
    
    this->levelLoaderListener->levelLoadUpdate("Carving...");
    
    this->carveTunnels();
    
    level->setData(width, depth, height, this->blocks);
    level->creationTime = static_cast<long>(time(nullptr));
    level->creator = username;
    level->name = "A Nice World";
    
    this->levelLoaderListener->levelLoadUpdate("Generation complete");
}

void LevelGen::generateMap() {
    PerlinNoiseFilter filter0(0);
    PerlinNoiseFilter filter1(1);
    
    std::vector<int> firstHeightMap = filter0.read(this->width, this->height);
    std::vector<int> secondHeightMap = filter0.read(this->width, this->height);
    std::vector<int> cliffMap = filter1.read(this->width, this->height);
    std::vector<int> rockMap = filter1.read(this->width, this->height);
    
    for (int x = 0; x < this->width; ++x) {
        for (int y = 0; y < this->depth; ++y) {
            for (int z = 0; z < this->height; ++z) {
                int firstHeightValue = firstHeightMap[x + z * this->width];
                int secondHeightValue = secondHeightMap[x + z * this->width];
                
                if (cliffMap[x + z * this->width] < 128) {
                    secondHeightValue = firstHeightValue;
                }
                
                int maxLevelHeight = std::max(secondHeightValue, firstHeightValue) / 8 + this->depth / 3;
                
                int maxRockHeight = rockMap[x + z * this->width] / 8 + this->depth / 3;
                
                if (maxRockHeight > maxLevelHeight - 2) {
                    maxRockHeight = maxLevelHeight - 2;
                }
                
                int index = (y * this->height + z) * this->width + x;
                
                int id = 0;
                
                if (y == maxLevelHeight) {
                    id = Tile::grass->id;
                }
                
                if (y < maxLevelHeight) {
                    id = Tile::dirt->id;
                }
                
                if (y <= maxRockHeight) {
                    id = Tile::rock->id;
                }
                
                this->blocks[index] = static_cast<uint8_t>(id);
            }
        }
    }
}

void LevelGen::carveTunnels() {
    int w = this->width;
    int h = this->height;
    int d = this->depth;
    int count = w * h * d / 256 / 64;
    
    for (int i = 0; i < count; ++i) {
        float x = this->random->nextFloat() * w;
        float y = this->random->nextFloat() * d;
        float z = this->random->nextFloat() * h;
        int length = static_cast<int>((this->random->nextFloat() + this->random->nextFloat()) * 150.0f);
        
        float yaw = this->random->nextFloat() * M_PI * 2.0f;
        float pitch = 0.0f;
        float yawMod = 0.0f;
        float pitchMod = 0.0f;
        
        for (int l = 0; l < length; ++l) {
            x += std::sin(yaw) * std::cos(pitch);
            z += std::cos(yaw) * std::cos(pitch);
            y += std::sin(pitch);
            
            yaw += yawMod * 0.2f;
            yawMod = (yawMod * 0.9f) + (this->random->nextFloat() - this->random->nextFloat());
            pitch = (pitch + pitchMod * 0.5f) * 0.5f;
            pitchMod = (pitchMod * 0.9f) + (this->random->nextFloat() - this->random->nextFloat());
            
            float size = std::sin(l * M_PI / length) * 2.5f + 1.0f;
            
            for (int xx = static_cast<int>(std::floor(x - size)); xx <= static_cast<int>(std::floor(x + size)); ++xx) {
                for (int yy = static_cast<int>(std::floor(y - size)); yy <= static_cast<int>(std::floor(y + size)); ++yy) {
                    for (int zz = static_cast<int>(std::floor(z - size)); zz <= static_cast<int>(std::floor(z + size)); ++zz) {
                        float xd = xx - x;
                        float yd = yy - y;
                        float zd = zz - z;
                        float distSq = xd * xd + yd * yd * 2.0f + zd * zd;
                        
                        if (distSq < size * size && 
                            xx >= 1 && yy >= 1 && zz >= 1 && 
                            xx < this->width - 1 && yy < this->depth - 1 && zz < this->height - 1) {
                            
                            int index = (yy * this->height + zz) * this->width + xx;
                            if (this->blocks[index] == Tile::rock->id) {
                                this->blocks[index] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

int LevelGen::getCoordIndex(int x, int y, int z) {
    return (y * this->height + z) * this->width + x;
}

std::vector<int> LevelGen::unpackCoord(int index) {
    int x = index % this->width;
    int temp = index / this->width;
    int z = temp % this->height;
    int y = temp / this->height;
    return {x, y, z};
}
