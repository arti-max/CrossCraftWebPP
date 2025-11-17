#pragma once
#include "Entity.hpp"
#include "character/ZombieModel.hpp"
#include "render/Textures.hpp"
#include "gui/Font.hpp"
#include "Player.hpp"
#include <string>

class NetworkPlayer : public Entity {
private:
    static const long long serialVersionUID = 77479605454997290;
    ZombieModel* model = new ZombieModel();
    int ticks = 0;
    int playerId;

    float bob;
    float bobo;
    int movingTicks;

    float timeOffs;
    float walkDist;
    float walkDistO;

    float serverX, serverY, serverZ;
    float serverYaw, serverPitch;

public:
    NetworkPlayer(Level* level, int playerId, std::string &username, float x, float y, float z, float yRot, float xRot);
    ~NetworkPlayer();

    std::string username;
    void tick() override;
    void render(Textures* tex, float partialTicks, Font* font, Player* localPlayer);

    void setServerPosition(float x, float y, float z, float yaw, float pitch);
};