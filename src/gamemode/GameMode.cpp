#include "gamemode/GameMode.hpp"
#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include "CrossCraft.hpp"
#include "player/Player.hpp"

GameMode::GameMode(CrossCraft* cc) {
    this->cc = cc;
    this->instantBreak = false;
}

void GameMode::apply(Level* level) {
    // nothing
}

void GameMode::openInventory() {
    // nothing
}

void GameMode::hitTile(int x, int y, int z) {
    this->breakTile(x, y, z);
}

bool GameMode::canPlace(int tile) {
    return true;
}

