#pragma once
#include <string>

class CrossCraft;

class v1Api {
private:
    CrossCraft* cc;
public:
    v1Api(CrossCraft* cc);

    // Only client-level
    bool setBlock(int x, int y, int z, int id, bool update=true);
    bool spawnMob(float x, float y, float z, int type);
    bool spawnItem(float x, float y, float z, int blockId);
    bool spawnArrow(float x, float y, float z, float yaw, float pitch, float speed);
    bool spawnSmolder(int x, int y, int z); // тление после взрыва из 0.16-0.17 survival test
    void setHitboxes(bool flag);
    int getEntityAt(int x, int y, int z);
    int getScore();
    void addScore(int points);
    // Client / Server-level:
    float getPlayerX();
    float getPlayerY();
    float getPlayerZ();
    int getBlock(int x, int y, int z);
    bool isSolidAt(int x, int y, int z);
    void sendToChat(const std::string& msg);
    bool spawnTerrainParticle(float x, float y, float z, float motionX, float motionY, float motionZ, int tileId);
    bool spawnSmokeParticle(float x, float y, float z);
    bool spawnWaterDropParticle(float x, float y, float z);
    void uploadTerrainTexture(const std::string& path);
    bool saveCustomData(const std::string& id, const std::string& data);
    std::string loadCustomData(const std::string& id);
};