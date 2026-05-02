#pragma once
#include "sound/SoundType.hpp"

class Player;
class Level;
class CrossCraft;
class Tile;

class GameMode {
public:
    bool instantBreak = false;
    CrossCraft* cc;

    GameMode(CrossCraft* cc);
    void apply(Level* level);
    void openInventory();
    void hitTile(int x, int y, int z);
    bool canPlace(int tile);
    void breakTile(int x, int y, int z);
    void hitTile(int x, int y, int z, int face);
    void resetHits();
    void applyCracks(float time);
    float getReachDistance();
    bool useItem(Player* player, int type);
    void preparePlayer(Player* player);
    void spawnMob();
    void prepareLevel();
    bool isSurvival();
    void apply();
};