#pragma once
#include <string>

class CrossCraft;

class v1Api {
private:
    CrossCraft* cc;
public:
    v1Api(CrossCraft* cc);

    bool setBlock(int x, int y, int z, int id);
    int getBlock(int x, int y, int z);
    bool isSolidAt(int x, int y, int z);
    bool spawnMob(float x, float y, float z, int type);
    bool spawnItem(float x, float y, float z, int blockId);
    bool spawnArrow(float x, float y, float z, float yaw, float pitch, float speed);
    void setHitboxes(bool flag);
    void sendToChat(std::string& msg);
    int getEntityAt(int x, int y, int z);
    bool saveCustomData(std::string& id, std::string& data);
    std::string& loadCustomData(std::string& id);
};