#pragma once
#include "Entity.hpp"
#include "character/ZombieModel.hpp"
#include "render/Textures.hpp"
#include "gui/Font.hpp"
#include "player/Player.hpp"
#include <string>
#include <deque>
#include <cmath>

struct NetworkPosition {
    float x, y, z;
    float yaw, pitch;
    
    NetworkPosition(float x, float y, float z, float yaw, float pitch) 
        : x(x), y(y), z(z), yaw(yaw), pitch(pitch) {}
};

class NetworkPlayer : public Entity {
private:
    static const long long serialVersionUID = 77479605454997290;
    ZombieModel* model = new ZombieModel();
    std::deque<NetworkPosition> moveQueue;

    int ticks = 0;
    int playerId;

    float animStep;
    float animStepO;
    
    float yBodyRot;
    float yBodyRotO;
    
    float run;
    float oRun;

    int textureId = -1;

public:
    NetworkPlayer(Level* level, int playerId, std::string &username, float x, float y, float z, float yRot, float xRot);
    ~NetworkPlayer();

    std::string username;
    std::string displayName;

    void tick() override;
    void render(Textures* tex, float partialTicks, Font* font, Player* localPlayer);

    void setServerPosition(float x, float y, float z, float yaw, float pitch);

    void queue(float x, float y, float z, float yaw, float pitch);
    void queue(float x, float y, float z);
    void queue(float yaw, float pitch);
};