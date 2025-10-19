#pragma once
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include "util/Random.hpp"
#include "phys/AABB.hpp"
#include "level/render/LevelListener.hpp"

class Level {
private:
    static const int TILE_UPDATE_INTERVAL = 200;
    static const int multiplier = 1664525;
    static const int addend = 1013904223;
    
    std::vector<LevelListener*> levelListeners;
    Random* random;
    std::set<int> ticking;
    static const int maxBits = 10;
    int randValue;
    int unprocessed = 0;
    
    void calcLightDepths(int x0, int z0, int x1, int z1);
    void neighborChanged(int x, int y, int z, int type);
    bool isLiquidTile(int tileId);
    bool isActiveLiquidTile(int tileId);
    int encodePosition(int x, int y, int z);
    void decodePosition(int code, int& x, int& y, int& z);

public:
    int width, height, depth;
    std::vector<uint8_t> blocks;
    std::vector<int> lightDepths;
    std::string name;
    std::string creator;
    long long creationTime = 0;

    Level(int width, int height, int depth);
    ~Level() = default;
    void generateCaves();
    void setData(int w, int d, int h, const std::vector<uint8_t>& blocks);
    float getGroundLevel() const { return 32.0f; }
    void tick();
    bool isTile(int x, int y, int z);
    bool isSolidTile(int x, int y, int z);
    bool isLightBlocker(int x, int y, int z);
    float getBrightness(int x, int y, int z);
    bool isLit(int x, int y, int z);
    int getTile(int x, int y, int z);
    bool setTile(int x, int y, int z, int type);
    bool setTileNoUpdate(int x, int y, int z, int type);
    std::vector<AABB> getCubes(const AABB& boundingBox);
    bool containsAnyLiquid(const AABB& box);
    bool containsLiquid(const AABB& box, int liquidId);
    void addListener(LevelListener* listener);
    void removeListener(LevelListener* listener);
    void addTick(int x, int y, int z);
    void removeTick(int x, int y, int z);
    bool needsTick(int tileId);

private:
    void generateMap();
};
