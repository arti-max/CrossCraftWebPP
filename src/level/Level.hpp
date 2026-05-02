#pragma once
#include <vector>
#include <string>
#include <set>
#include <deque>
#include <cmath>
#include <map>
#include "util/Random.hpp"
#include "phys/AABB.hpp"
#include "level/render/LevelListener.hpp"
#include "level/EntityMesh.hpp"
#include "level/EntityMeshSlot.hpp"
#include "level/liquid/LiquidType.hpp"

class NetworkPlayer;
class Entity;

struct TickEntry {
    int x = 0, y = 0, z = 0, tileId = 0;
};

class CrossCraft;

class Level {
private:
    static const int TILE_UPDATE_INTERVAL = 200;
    static const int multiplier = 1664525;
    static const int addend = 1013904223;
    
    std::vector<LevelListener*> levelListeners;
    std::set<int> ticking;
    std::deque<TickEntry> tickNextTickList;
    std::vector<TickEntry> bannedTiles;
    std::vector<Entity*> pendingAdd;

    static const int maxBits = 10;
    int randValue = 0;
    int unprocessed = 0;
    
    void calcLightDepths(int x0, int z0, int x1, int z1);
    void neighborChanged(int x, int y, int z, int type);
    bool isLiquidTile(int tileId);
    bool isActiveLiquidTile(int tileId);
    int encodePosition(int x, int y, int z);
    void decodePosition(int code, int& x, int& y, int& z);

public:
    int width = 0, height = 0, depth = 0;
    int tickCount = 0;
    std::vector<uint8_t> blocks;
    std::vector<int> lightDepths;
    std::vector<Entity*> entities;
    std::map<int, NetworkPlayer*> networkPlayers;
    std::string name = "";
    std::string creator = "";
    long long creationTime = 0;
    Entity* player = nullptr;

    Random* random;
    CrossCraft* cc = nullptr;

    EntityMesh* emesh;

    int xSpawn = 0;
    int ySpawn = 0;
    int zSpawn = 0;
    int rotSpawn = 0;

    bool isRemote = false;

    Level();
    ~Level() = default;
    void generateCaves();
    void setData(int w, int d, int h, const std::vector<uint8_t>& blocks);
    float getGroundLevel() const;
    float getWaterLevel() const;
    void tick();
    void tickEntities();
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
    bool containsLiquid(const AABB& box, LiquidType liquidId);
    void addListener(LevelListener* listener);
    void removeListener(LevelListener* listener);
    void addTick(int x, int y, int z);
    void removeTick(int x, int y, int z);
    bool needsTick(int tileId);
    void addToTickNextTick(int x, int y, int z, int tileId);
    void swap(int x1, int y1, int z1, int x2, int y2, int z2);
    void initTransient();
    void findSpawn();
    void setSpawnPos(int x, int y, int z, int rot);
    int getHighestTile(int x, int z);
    void fillOcean(int x, int y, int z);
    void addBanned(int x, int y, int z, int id);
    void removeBanned(int x, int y, int z, int id);
    bool isBanned(int x, int y, int z);
    void playSound(const std::string& name, Entity* entity, float volume, float pitch);
    void playSound(const std::string& name, float x, float y, float z, float volume, float pitch);
    void playSound(const std::string& name, float volume, float pitch);
    void addEntity(Entity* e);
    void removeEntity(Entity* e);
    std::vector<Entity*> findEntities(Entity* e, const AABB& bbox);
    Entity* getPlayer();

private:
    void generateMap();
};
