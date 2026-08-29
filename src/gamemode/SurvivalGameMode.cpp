#include "gamemode/SurvivalGameMode.hpp"
#include "CrossCraft.hpp"
#include "Logger.hpp"
#include "MobSpawner.hpp"
#include "gamemode/GameMode.hpp"
#include "level/Level.hpp"
#include "player/Player.hpp"

SurvivalGameMode::SurvivalGameMode(CrossCraft* cc) : GameMode(cc) {}

void SurvivalGameMode::apply(Level* level) {
    GameMode::apply(level);

    this->spawner = new MobSpawner(level);
}

void SurvivalGameMode::hitTile(int x, int y, int z, int side) {
    if (this->hitDelay > 0) {
        --this->hitDelay;
    } else if (x == this->hitX && y == this->hitY && z == this->hitZ) {
        int type = this->cc->level->getTile(x, y, z);

        if (type != 0) {
            Tile* tile = Tile::tiles[type];
            this->hardness = tile->getHardness();
            tile->spawnHitParticles(cc->level, x, y, z, side, cc->particleEngine);
            this->hits++;

            if (this->hits == hardness+1) {
                this->breakTile(x, y, z);

                this->hits = 0;
                this->hitDelay = 5;
            }
        }
    } else {
        this->hits = 0;
        this->hitX = x;
        this->hitY = y;
        this->hitZ = z;
    }
}

void SurvivalGameMode::breakTile(int x, int y, int z) {
    int tile = this->cc->level->getTile(x, y, z);
    Tile::tiles[tile]->onDestroy(this->cc->level, x, y, z);

    GameMode::breakTile(x, y, z);
}

void SurvivalGameMode::hitTile(int x, int y, int z) {
    int tile = this->cc->level->getTile(x, y, z);

    if (tile > 0 && Tile::tiles[tile]->getHardness() == 0) {
        this->breakTile(x, y, z);
    }
}

void SurvivalGameMode::applyCracks(float time) {
    if (this->hits <= 0) {
        cc->levelRenderer->cracks = 0.0f;
    } else {
        cc->levelRenderer->cracks = ((float)this->hits + time - 1.0f) / (float)this->hardness;
    }
}

void SurvivalGameMode::resetHits() {
    this->hits = 0;
    this->hitDelay = 0;
}

bool SurvivalGameMode::canPlace(int id) {
    return cc->player->inventory->removeItem(id);
}

float SurvivalGameMode::getReachDistance() {
    return 4.0f;
}

void SurvivalGameMode::preparePlayer(Player* player) {
    player->inventory->slots[8] = Tile::tnt->id;
    player->inventory->count[8] = 10;
    // player->inventory->slots[0] = Tile::brownMushroom->id;
    // player->inventory->count[0] = 10;
    // player->inventory->slots[1] = Tile::redMushroom->id;
    // player->inventory->count[1] = 10;
}

bool SurvivalGameMode::isSurvival() {
    return true;
}

bool SurvivalGameMode::useItem(Player* player, int id) {
    Tile* tile = Tile::tiles[id];
    if (tile->id == Tile::redMushroom->id && player->inventory->removeItem(id)) {
        player->hurt(nullptr, 3);
        return true;
    } else if (tile->id == Tile::brownMushroom->id && player->inventory->removeItem(id)) {
        player->heal(5);
        return true;
    } else {
        return false;
    }
}
void SurvivalGameMode::prepareLevel(Level* level) {
    GameMode::prepareLevel(level);
    this->cc->levelLoadUpdate("Spawning..");
    int area = level->width * level->height * level->depth / 800;
    this->spawner->spawn(area, nullptr, this->cc);
}

void SurvivalGameMode::spawnMob() {
    Level* level = this->cc->level;

    int area = level->width * level->height * level->depth / 64 / 64 / 64;

    if (level->random->nextInt(100) < area && level->emesh->getMobCount() < 20 * area) {
        this->spawner->spawn(area, level->player, nullptr);
    }
}