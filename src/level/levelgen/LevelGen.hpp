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

    void generateMap();
    int getCoordIndex(int x, int y, int z);
    std::vector<int> unpackCoord(int id);

public:
    LevelGen(LevelLoaderListener* listener);
    ~LevelGen();

    void generateLevel(Level* level, const char useranme[], int width, int height, int depth);
    void carveTunnels();
};