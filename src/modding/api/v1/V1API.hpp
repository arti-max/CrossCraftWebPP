#pragma once
#include <string>

class CrossCraft;

class v1Api {
private:
    CrossCraft* cc;
public:
    v1Api(CrossCraft* cc);

    bool setBlock(int x, int y, int z, int id, bool update=true);
    int getBlock(int x, int y, int z);
    bool isSolidAt(int x, int y, int z);
    bool spawnMob(float x, float y, float z, int type);
    bool spawnItem(float x, float y, float z, int blockId);
    bool spawnArrow(float x, float y, float z, float yaw, float pitch, float speed);
    void setHitboxes(bool flag);
    void sendToChat(const std::string& msg);
    int getEntityAt(int x, int y, int z);
    int getScore();
    void addScore(int points);
    float getPlayerX();
    float getPlayerY();
    float getPlayerZ();
    void uploadTerrainTexture(const std::string& path); // единственный метод, дсотупный в мультиплеере
    bool saveCustomData(const std::string& id, const std::string& data);
    std::string loadCustomData(const std::string& id);
};