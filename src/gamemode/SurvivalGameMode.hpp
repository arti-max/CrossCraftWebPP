#pragma once
#include "gamemode/GameMode.hpp"

class SurvivalGameMode : public GameMode {
public:
    int hitX = 0;
    int hitY = 0;
    int hitZ = 0;
    int hits = 0;
    int hardness = 0;
    int hitDelay = 0;
    int gmType = 0;

    SurvivalGameMode(CrossCraft* cc);
    void apply(Level* level) override;
    void hitTile(int x, int y, int z, int face) override;
    bool canPlace(int tile) override;
    void breakTile(int x, int y, int z) override;
    void hitTile(int x, int y, int z) override;
    void resetHits() override;
    void applyCracks(float time) override;
    float getReachDistance() override;
    bool useItem(Player* player, int type) override;
    void preparePlayer(Player* player) override;
    void spawnMob() override;
    void prepareLevel(Level* level) override;
};