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

void GameMode::breakTile(int x, int y, int z) {
    Level* level = this->cc->level;
    Tile* tile = Tile::tiles[level->getTile(x, y, z)];

    bool placed = level->setTile(x, y, z, 0);

    if (tile != nullptr && placed) {
        if (tile->st != &SoundType::none) {
            level->playSound("step." + tile->st->name, (float)x, (float)y, (float)z, (tile->st->getVolume() + 1.0f) / 2.0f, tile->st->getPitch() * 0.8f);
        }


        tile->onDestroy(level, x, y, z, cc->particleEngine, true);
    }
}

void GameMode::hitTile(int x, int y, int z, int face) {
    // nothing
}

void GameMode::resetHits() {
    // nothing
}

void GameMode::applyCracks(float time) {
    // nothing
}

bool GameMode::useItem(Player* player, int type) {
    return false;
}

void GameMode::preparePlayer(Player* player) {
    // nothing
}

void GameMode::spawnMob() {
    // nothing
}

void GameMode::prepareLevel() {
    // nothing
}

bool isSurvival() {
    return true;
}

void apply(Player* player) {
    // nothing
}