#pragma once
#include "Entity.hpp"
#include "character/ZombieModel.hpp"
#include "render/Textures.hpp"
#include <string>

class NetworkPlayer : public Entity {
private:
    static const long long serialVersionUID = 77479605454997290;
    ZombieModel* model = new ZombieModel();
    int ticks = 0;

    float bob;
    float bobo;
    int movingTicks;

    float timeOffs;
    float walkDist;
    float walkDistO;

    float serverX, serverY, serverZ;
    float serverYaw, serverPitch;

public:
    NetworkPlayer(Level* level, int var2, std::string &var3, float x, float y, float z, float yRot, float xRot);
    ~NetworkPlayer();
    void tick() override;
    void render(Textures* tex, float partialTicks);

    void setServerPosition(float x, float y, float z, float yaw, float pitch);
};