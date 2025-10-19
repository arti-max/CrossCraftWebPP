#pragma once
#include "level/LevelLoaderListener.hpp"
#include "level/levelgen/PerlinNoiseFilter.hpp"
#include "util/Random.hpp"
#include "level/Level.hpp"
#include <vector>
#include <array>

class LevelGen {
private:
    LevelLoaderListener* levelLoaderListener = nullptr;

    int width = 0;
    int height = 0;
    int depth = 0;
    Random* random = new Random();
    std::vector<uint8_t> blocks;
    
    std::vector<int> coords;
    static const int COORD_BUFFER_SIZE = 1048576;

    void generateMap();
    int getCoordIndex(int x, int y, int z);
    std::vector<int> unpackCoord(int index);
    
    void addWater();
    void addLava();
    int floodFillLiquid(int x, int y, int z, uint8_t source, uint8_t target);

public:
    LevelGen(LevelLoaderListener* listener);
    ~LevelGen();

    void generateLevel(Level* level, const char username[], int width, int height, int depth);
    void carveTunnels();
};
