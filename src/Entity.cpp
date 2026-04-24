#include "Entity.hpp"
#include "level/Level.hpp"
#include "util/Logger.hpp"
#include "sound/SoundType.hpp"
#include "level/tile/Tile.hpp"
#include "level/EntityMesh.hpp"
#include <random>

std::mt19937 Entity::randomGenerator(std::random_device{}());

Entity::Entity(Level* level) : level(level), bb(0, 0, 0, 0, 0, 0) {
    xo = yo = zo = 0.0f;
    x = y = z = 0.0f;
    xd = yd = zd = 0.0f;
    yRot = xRot = 0.0f;
    
    this->resetPos();
}

void Entity::resetPos() {
    float x = (float)this->level->xSpawn + 0.5f;
    float y = (float)this->level->ySpawn;

    for (float z = (float)this->level->zSpawn + 0.5f; y > 0.0f; ++y) {
        this->setPos(x, y, z);
        if (this->level->getCubes(this->bb).size() == 0) {
            break;
        }
        if (y > this->level->depth + 10) { 
            break;
        }
    }
    
    this->xd = this->yd = this->zd = 0.0f;
    this->yRot = this->level->rotSpawn;
    this->xRot = 0.0f;
}

void Entity::remove() {
    removed = true;
}

void Entity::setSize(float w, float h) {
    bbWidth = w;
    bbHeight = h;
}

void Entity::setRot(float yRot, float xRot) {
    while(this->yRotO - yRot < -180.0F) {
        this->yRotO += 360.0F;
    }

    while(this->yRotO - yRot >= 180.0F) {
        this->yRotO -= 360.0F;
    }

    this->yRot = yRot;
    this->xRot = xRot;
   }

void Entity::setPos(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    float w = bbWidth / 2.0f;
    float h = bbHeight / 2.0f;
    bb = AABB(x - w, y - h, z - w, x + w, y + h, z + w);
}

void Entity::turn(float xo, float yo) {
    yRot += xo * 0.15f;
    xRot -= yo * 0.15f;
    
    xRot = std::max(-90.0f, xRot);
    xRot = std::min(90.0f, xRot);
}

void Entity::interpolateTurn(float xo, float yo) {
    float oxr = this->xRot;
    float oyr = this->yRot;
    this->yRot = (float)((double)this->yRot + (double)xo * 0.15f);
    this->xRot = (float)((double)this->xRot - (double)yo * 0.15f);
    this->xRot = std::max(-90.0f, this->xRot);
    this->xRot = std::min(90.0f, this->xRot);

    this->xRotO = this->xRot - oxr;
    this->yRotO = this->yRot - oyr;
}

void Entity::tick() {
    xo = x;
    yo = y;
    zo = z;
    this->xRotO = this->xRot;
    this->yRotO = this->yRot;
}

bool Entity::isFree(float xa, float ya, float za) {
    AABB box = bb.cloneMove(xa, ya, za);
    std::vector<AABB> aabbs = level->getCubes(box);
    
    if (aabbs.size() > 0) {
        // Logger::logf(PREFIX_DEBUG, "isFree [1]  ");
        return false;
    } else {
        // Logger::logf(PREFIX_DEBUG, "isFree [2]  ");
        return !level->containsAnyLiquid(box);
    }
}

void Entity::move(float xa, float ya, float za) {
    float xxo = this->x;
    float zzo = this->z;
    float xaOrg = xa;
    float yaOrg = ya;
    float zaOrg = za;
    
    std::vector<AABB> aabbs = level->getCubes(bb.expand(xa, ya, za));

    for (size_t i = 0; i < aabbs.size(); ++i) {
        ya = aabbs[i].clipYCollide(bb, ya);
    }
    bb.move(0.0f, ya, 0.0f);

    for (size_t i = 0; i < aabbs.size(); ++i) {
        xa = aabbs[i].clipXCollide(bb, xa);
    }
    bb.move(xa, 0.0f, 0.0f);

    for (size_t i = 0; i < aabbs.size(); ++i) {
        za = aabbs[i].clipZCollide(bb, za);
    }
    bb.move(0.0f, 0.0f, za);

    this->horizontalCollision = xaOrg != xa || zaOrg != za;
    this->onGround = (yaOrg != ya && yaOrg < 0.0f);

    if (xaOrg != xa) {
        xd = 0.0f;
    }
    if (yaOrg != ya) {
        yd = 0.0f;
    }
    if (zaOrg != za) {
        zd = 0.0f;
    }

    x = (bb.x0 + bb.x1) / 2.0f;
    y = bb.y0 + heightOffset;
    z = (bb.z0 + bb.z1) / 2.0f;
    float zz = this->x - xxo;
    xa = this->z - zzo;
    this->walkDist = (float)((float)this->walkDist+std::sqrt((float)(zz*zz+xa*xa))*0.6f);
    if (this->makeStepSound) {
        int tileid = this->level->getTile((int)this->x, (int)(this->y - 0.2f - this->heightOffset), (int)this->z);
        const SoundType* st = Tile::tiles[tileid]->st;
        if (this->walkDist > 1.0f && tileid > 0 && st != &SoundType::none) {
            // Logger::logf(PREFIX_DEBUG, "STEP! tile: %d, sound: %s\n", tileid, st->name.c_str());
            this->walkDist -= (float)((int)this->walkDist);
            this->playSound("step." + st->name, st->getVolume()*0.75f, st->getPitch());
        }
    }

}

bool Entity::isInWater() {
    return level->containsLiquid(bb.grow(0.0f, -0.4f, 0.0f), 1);
}

bool Entity::isInLava() {
    return level->containsLiquid(bb, 2);
}

void Entity::moveRelative(float xa, float za, float speed) {
    float dist = xa * xa + za * za;
    
    if (dist >= 0.01f) {
        dist = speed / std::sqrt(dist);
        xa *= dist;
        za *= dist;
        
        float yRotRadians = yRot * static_cast<float>(M_PI) / 180.0f;
        float sinYRot = std::sin(yRotRadians);
        float cosYRot = std::cos(yRotRadians);
        
        xd += xa * cosYRot - za * sinYRot;
        zd += za * cosYRot + xa * sinYRot;
    }
}

bool Entity::isLit() {
    int xTile = static_cast<int>(x);
    int yTile = static_cast<int>(y);
    int zTile = static_cast<int>(z);
    
    return level->isLit(xTile, yTile, zTile);
}

void Entity::render(float partialTicks, Textures* textures) {
}

float Entity::randomFloat() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(randomGenerator);
}

float Entity::getBrightness() {
    int xTile = static_cast<int>(x);
    int yTile = static_cast<int>(y + heightOffset / 2.0f);
    int zTile = static_cast<int>(z);
    return level->getBrightness(xTile, yTile, zTile);
}

void Entity::playSound(const std::string& name, float volume, float pitch) {
    if (this->isPlayer()) {
        this->level->playSound(name, volume, pitch);
    } else {
        this->level->playSound(name, this, volume, pitch);
    }
}

bool Entity::isPlayer() {
    return false;
}

bool Entity::intersects(float x0, float y0, float z0, float x1, float y1, float z1) {
    return this->bb.intersects(x0, y0, z0, x1, y1, z1);
}

void Entity::setLevel(Level* level) {
    this->level = level;
}