#include "modding/api/v1/V1API.hpp"
#include "CrossCraft.hpp"

v1Api::v1Api(CrossCraft *cc) : cc(cc) {

}

float v1Api::getPlayerX() {
    return this->cc->player->x;
}

float v1Api::getPlayerY() {
    return this->cc->player->y;
}

float v1Api::getPlayerZ() {
    return this->cc->player->z;
}

bool v1Api::setBlock(int x, int y, int z, int tileId, bool update) {
    if (update) {
        return this->cc->level->setTile(x, y, z, tileId);
    }
    return this->cc->level->setTileNoUpdate(x, y, z, tileId);
}

int v1Api::getBlock(int x, int y, int z) {
    return this->cc->level->getTile(x, y, z);
}

bool v1Api::isSolidAt(int x, int y, int z) {
    return this->cc->level->isSolidTile(x, y, z);
}