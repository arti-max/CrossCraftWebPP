#include "LevelGen.hpp"
#include "level/levelgen/synth/OctaveNoise.hpp"
#include "level/levelgen/synth/CombinedNoise.hpp"
#include "level/tile/Tile.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <chrono>

LevelGen::LevelGen(LevelLoaderListener* listener) : listener(listener), random() {}

LevelGen::~LevelGen() = default;

void LevelGen::generateLevel(Level* level, const char* username, int w, int h, int d) {
    listener->beginLevelLoading("Generating level");
    this->width = w;
    this->depth = d;
    this->height = h;

    this->blocks.assign(width * height * depth, 0);
    this->heightmap.assign(width * height, 0);
    
    listener->levelLoadUpdate("Raising..");
    raise(this->heightmap);

    listener->levelLoadUpdate("Eroding..");
    erode(this->heightmap);

    listener->levelLoadUpdate("Soiling..");
    soil(this->heightmap);

    listener->levelLoadUpdate("Carving..");
    carve();

    listener->levelLoadUpdate("Watering..");
    addWaterAndLava();

    listener->levelLoadUpdate("Growing..");
    growSurface(this->heightmap);
    
    listener->levelLoadUpdate("Planting..");
    addTrees(this->heightmap);

        listener->levelLoadUpdate("Finalizing..");
    listener->levelLoadProgress(50);

    level->setData(width, depth, height, this->blocks);
    level->initTransient();
    level->creationTime = time(nullptr);
    level->creator = username;
    level->name = "A Nice World";
    
    listener->levelLoadUpdate("Done");
    listener->levelLoadProgress(100);
}

void LevelGen::raise(std::vector<int>& map) {
    CombinedNoise noise1(
        std::make_unique<OctaveNoise>(random, 8),
        std::make_unique<OctaveNoise>(random, 8)
    );
    CombinedNoise noise2(
        std::make_unique<OctaveNoise>(random, 8),
        std::make_unique<OctaveNoise>(random, 8)
    );
    OctaveNoise cliffNoise(random, 8);
    
    float scale = 1.3f;

    for (int x = 0; x < width; ++x) {
        listener->levelLoadProgress(x * 100 / (width - 1));
        for (int z = 0; z < height; ++z) {
            double h1 = noise1.compute(x * scale, z * scale) / 8.0 - 8.0;
            double h2 = noise2.compute(x * scale, z * scale) / 6.0 + 6.0;

            if (cliffNoise.compute(x, z) / 8.0 > 0.0) {
                h2 = h1;
            }

            double val = std::max(h1, h2) / 2.0;
            if (val < 0.0) val /= 2.0;
            
            map[x + z * width] = static_cast<int>(val);
        }
    }
}

void LevelGen::erode(std::vector<int>& map) {
    CombinedNoise erosionNoise1(std::make_unique<OctaveNoise>(random, 8), std::make_unique<OctaveNoise>(random, 8));
    CombinedNoise erosionNoise2(std::make_unique<OctaveNoise>(random, 8), std::make_unique<OctaveNoise>(random, 8));

    for (int x = 0; x < width; ++x) {
        listener->levelLoadProgress(x * 100 / (width - 1));
        for (int z = 0; z < height; ++z) {
            double val = erosionNoise1.compute(x * 2.0, z * 2.0) / 8.0;
            int erosion = erosionNoise2.compute(x * 2.0, z * 2.0) > 0.0 ? 1 : 0;

            if (val > 2.0) {
                int h = map[x + z * width];
                h = ((h - erosion) / 2) * 2 + erosion;
                map[x + z * width] = h;
            }
        }
    }
}

void LevelGen::soil(std::vector<int>& map) {
    OctaveNoise soilNoise(random, 8);
    int waterLevel = depth / 2;

    for (int x = 0; x < width; ++x) {
        listener->levelLoadProgress(x * 100 / (width - 1));
        for (int z = 0; z < height; ++z) {
            int soilDepth = static_cast<int>(soilNoise.compute(x, z) / 24.0) - 4;
            
            int dirtTopY = map[x + z * width] + waterLevel;
            int rockTopY = dirtTopY + soilDepth;
            map[x + z * width] = std::max(dirtTopY, rockTopY);

            for (int y = 0; y < depth; ++y) {
                int index = (y * height + z) * width + x;
                uint8_t id = 0;

                if (y <= dirtTopY) {
                    id = Tile::dirt->id;
                }

                if (y <= rockTopY) {
                    id = Tile::rock->id;
                }

                blocks[index] = id;
            }
        }
    }
}

void LevelGen::growSurface(const std::vector<int>& map) {
    OctaveNoise sandNoise(random, 8);
    OctaveNoise gravelNoise(random, 8);
    int waterLevel = depth / 2;

    for (int x = 0; x < width; ++x) {
        listener->levelLoadProgress(x * 100 / (width - 1));
        for (int z = 0; z < height; ++z) {
            bool placeSand = sandNoise.compute(x, z) > 8.0;
            bool placeGravel = gravelNoise.compute(x, z) > 12.0;
            
            int surfaceY = map[x + z * width];

            if (surfaceY < 0 || surfaceY >= depth - 1) continue;
            
            int index = (surfaceY * height + z) * width + x;
            int indexAbove = ((surfaceY + 1) * height + z) * width + x;
            
            if (blocks[indexAbove] == 0) {
                uint8_t surfaceID = Tile::grass->id;

                if (surfaceY <= waterLevel && placeGravel) {
                    surfaceID = Tile::gravel->id; 
                }
                if (surfaceY <= waterLevel && placeSand) {
                    surfaceID = Tile::sand->id;
                }
                
                blocks[index] = surfaceID;
            }
        }
    }
}

void LevelGen::carve() {
    int numCaves = width * height * depth / 256 / 64;
    for (int i = 0; i < numCaves; ++i) {
        listener->levelLoadProgress(i * 100 / (numCaves - 1));
        float x = random.nextFloat() * width;
        float y = random.nextFloat() * depth;
        float z = random.nextFloat() * height;
        
        int length = static_cast<int>((random.nextFloat() + random.nextFloat()) * 75.0f);
        
        float yaw = random.nextFloat() * 2.0f * M_PI;
        float pitch = 0.0f;
        float yawMod = 0.0f;
        float pitchMod = 0.0f;

        for (int l = 0; l < length; ++l) {
            x += std::sin(yaw) * std::cos(pitch);
            z += std::cos(yaw) * std::cos(pitch);
            y += std::sin(pitch);
            
            yaw += yawMod * 0.2f;
            yawMod = (yawMod * 0.9f) + (random.nextFloat() - random.nextFloat());
            pitch = (pitch + pitchMod * 0.5f) * 0.5f;
            pitchMod = (pitchMod * 0.9f) + (random.nextFloat() - random.nextFloat());
            
            float size = std::sin(static_cast<float>(l) * M_PI / static_cast<float>(length)) * 2.5f + 1.0f;

            for (int ix = static_cast<int>(x - size); ix <= static_cast<int>(x + size); ++ix) {
                for (int iy = static_cast<int>(y - size); iy <= static_cast<int>(y + size); ++iy) {
                    for (int iz = static_cast<int>(z - size); iz <= static_cast<int>(z + size); ++iz) {
                        float dx = ix - x;
                        float dy = iy - y;
                        float dz = iz - z;

                        if (dx * dx + dy * dy * 2.0f + dz * dz < size * size &&
                            ix >= 1 && iy >= 1 && iz >= 1 && 
                            ix < width - 1 && iy < depth - 1 && iz < height - 1) {
                            
                            int index = (iy * height + iz) * width + ix;
                            if (blocks[index] == Tile::rock->id) {
                                blocks[index] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    listener->levelLoadUpdate("Adding minerals..");
    listener->levelLoadProgress(25);
    addVeins(Tile::coalOre->id, 90);
    listener->levelLoadProgress(50);
    addVeins(Tile::ironOre->id, 70);
    listener->levelLoadProgress(75);
    addVeins(Tile::goldOre->id, 50);
    listener->levelLoadProgress(100);
}

void LevelGen::addWaterAndLava() {
    int waterLevel = depth / 2;
    int filled = 0;
    
    for (int x = 0; x < width; ++x) {
        floodFill(x, waterLevel - 1, 0, Tile::water->id);
        floodFill(x, waterLevel - 1, height - 1, Tile::water->id);
    }
    for (int z = 0; z < height; ++z) {
        floodFill(0, waterLevel - 1, z, Tile::water->id);
        floodFill(width - 1, waterLevel - 1, z, Tile::water->id);
    }
    
    int waterSources = width * height / 200;
    for (int i = 0; i < waterSources; ++i) {
        listener->levelLoadProgress(i * 100 / (waterSources - 1));
        int x = random.nextInt(width);
        int y = waterLevel - 1 - random.nextInt(2);
        int z = random.nextInt(height);
        if(blocks[(y * height + z) * width + x] == 0)
            floodFill(x, y, z, Tile::water->id);
    }

    listener->levelLoadUpdate("Melting..");
    int lavaSources = width * height * depth / 10000;
    for (int i = 0; i < lavaSources; i++) {
        listener->levelLoadProgress(i * 100 / (lavaSources - 1));
        int x = random.nextInt(width);
        int y = random.nextInt(depth / 4);
        int z = random.nextInt(height);
        if (blocks[(y * height + z) * width + x] == 0) {
            floodFill(x, y, z, Tile::lava->id);
        }
    }
}


void LevelGen::floodFill(int x, int y, int z, uint8_t targetBlock) {
    std::vector<int> stack;
    stack.reserve(width * height);
    stack.push_back((y * height + z) * width + x);

    while(!stack.empty()){
        int index = stack.back();
        stack.pop_back();

        if(blocks[index] == 0){
            blocks[index] = targetBlock;

            int ix = index % width;
            int temp = index / width;
            int iz = temp % height;
            int iy = temp / height;
            
            if(ix > 0) stack.push_back(index - 1);
            if(ix < width - 1) stack.push_back(index + 1);
            if(iz > 0) stack.push_back(index - width);
            if(iz < height - 1) stack.push_back(index + width);
            if(iy > 0) stack.push_back(index - width * height);
        }
    }
}

void LevelGen::addTrees(const std::vector<int>& map) {
    int numTreePatches = width * height / 2000;

    for (int i = 0; i < numTreePatches; ++i) {
        listener->levelLoadProgress(i * 100 / (numTreePatches - 1));
        int patchX = random.nextInt(width);
        int patchZ = random.nextInt(height);

        for (int j = 0; j < 5; ++j) {
            int treeX = patchX;
            int treeZ = patchZ;

            for (int k = 0; k < 20; ++k) {
                treeX += random.nextInt(6) - random.nextInt(6);
                treeZ += random.nextInt(6) - random.nextInt(6);

                if (treeX > 1 && treeZ > 1 && treeX < width - 1 && treeZ < height - 1) {
                    
                    int bareTrunkHeight = 2 + random.nextInt(2);
                    int leafyTrunkHeight = 2;
                    int totalTrunkHeight = bareTrunkHeight + leafyTrunkHeight;
                    int treeY = map[treeX + treeZ * width] + 1;

                    int groundIndex = ((treeY - 1) * height + treeZ) * width + treeX;
                    if (treeY > 0 && treeY + totalTrunkHeight + 1 < depth && blocks[groundIndex] == Tile::grass->id) {
                        
                        bool canPlace = true;
                        for(int yOff = 0; yOff < totalTrunkHeight + 1; ++yOff) {
                            if (blocks[((treeY + yOff) * height + treeZ) * width + treeX] != 0) {
                                canPlace = false;
                                break;
                            }
                        }

                        if (canPlace) {

                            blocks[groundIndex] = Tile::dirt->id;

                            for (int yOff = 0; yOff < totalTrunkHeight; ++yOff) {
                                int trunkIndex = ((treeY + yOff) * height + treeZ) * width + treeX;
                                blocks[trunkIndex] = Tile::log->id;
                            }

                            for (int yOff = 0; yOff < leafyTrunkHeight; ++yOff) {
                                int currentLeafY = treeY + bareTrunkHeight + yOff;
                                for (int xOff = -1; xOff <= 1; ++xOff) {
                                    for (int zOff = -1; zOff <= 1; ++zOff) {
                                        if (xOff == 0 && zOff == 0) continue;
                                        int leafIndex = (currentLeafY * height + (treeZ + zOff)) * width + (treeX + xOff);
                                        if (blocks[leafIndex] == 0) blocks[leafIndex] = Tile::leaves->id;
                                    }
                                }
                            }

                            int topY = treeY + totalTrunkHeight;
                            #define SAFE_SET_LEAF(lx, lz) if(blocks[(topY * height + (lz)) * width + (lx)] == 0) blocks[(topY * height + (lz)) * width + (lx)] = Tile::leaves->id;
                            SAFE_SET_LEAF(treeX, treeZ);
                            SAFE_SET_LEAF(treeX + 1, treeZ);
                            SAFE_SET_LEAF(treeX - 1, treeZ);
                            SAFE_SET_LEAF(treeX, treeZ + 1);
                            SAFE_SET_LEAF(treeX, treeZ - 1);
                            #undef SAFE_SET_LEAF
                        }
                    }
                }
            }
        }
    }
}

void LevelGen::addVeins(int tileId, int abundance) {
    int numVeins = width * height * depth * abundance / 100 / 64 / 100;

    for (int i = 0; i < numVeins; ++i) {
        float x = random.nextFloat() * width;
        float y = random.nextFloat() * depth;
        float z = random.nextFloat() * height;
        
        int length = static_cast<int>((random.nextFloat() + random.nextFloat()) * 75.0f * (abundance / 100.0f));
        
        float yaw = random.nextFloat() * 2.0f * M_PI;
        float pitch = random.nextFloat() * 2.0f * M_PI;
        float yawMod = 0.0f;
        float pitchMod = 0.0f;

        for (int l = 0; l < length; ++l) {
            x += std::sin(yaw) * std::cos(pitch);
            z += std::cos(yaw) * std::cos(pitch);
            y += std::sin(pitch);
            
            yaw += yawMod * 0.2f;
            yawMod = (yawMod * 0.9f) + (random.nextFloat() - random.nextFloat());
            pitch = (pitch + pitchMod * 0.5f) * 0.5f;
            pitchMod = (pitchMod * 0.9f) + (random.nextFloat() - random.nextFloat());
            
            float size = std::sin(static_cast<float>(l) * M_PI / static_cast<float>(length)) * (abundance / 100.0f) + 1.0f;

            for (int ix = static_cast<int>(x - size); ix <= static_cast<int>(x + size); ++ix) {
                for (int iy = static_cast<int>(y - size); iy <= static_cast<int>(y + size); ++iy) {
                    for (int iz = static_cast<int>(z - size); iz <= static_cast<int>(z + size); ++iz) {
                        float dx = ix - x;
                        float dy = iy - y;
                        float dz = iz - z;

                        if (dx * dx + dy * dy * 2.0f + dz * dz < size * size &&
                            ix >= 1 && iy >= 1 && iz >= 1 && 
                            ix < width - 1 && iy < depth - 1 && iz < height - 1) {
                            
                            int index = (iy * height + iz) * width + ix;

                            if (blocks[index] == Tile::rock->id) {
                                blocks[index] = tileId;
                            }
                        }
                    }
                }
            }
        }
    }
}