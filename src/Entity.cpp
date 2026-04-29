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
    this->walkDistO = this->walkDist;
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
    // if (this->noPhysics) {
    //     this->bb.move(xa, ya, za);
    //     this->x = (this->bb.x0 + this->bb.x1) / 2.0f;
    //     this->y = (this->bb.y0 + this->heightOffset - this->ySlideOffset);
    //     this->z = (this->bb.z0 + this->bb.z1) / 2.0f;
    // } else{
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
    if (!this->slide && yaOrg != ya) {
        za = 0;
        ya = 0;
        xa = 0;
    }

    // bool grnd = this->onGround || yaOrg != ya && yaOrg < 0.0f;

    for (size_t i = 0; i < aabbs.size(); ++i) {
        xa = aabbs[i].clipXCollide(bb, xa);
    }

    bb.move(xa, 0.0f, 0.0f);
    if (!this->slide && xaOrg != xa) {
        za = 0;
        ya = 0;
        xa = 0;
    }

    for (size_t i = 0; i < aabbs.size(); ++i) {
        za = aabbs[i].clipZCollide(bb, za);
    }

    bb.move(0.0f, 0.0f, za);
    if (!this->slide && zaOrg != za) {
        za = 0;
        ya = 0;
        xa = 0;
    }

    // if (this->footSize > 0.0f && grnd && this->ySlideOffset < 0.05f && (xaOrg != xa || zaOrg != za)) {
    //     float xx = xa;
    //     float yy = ya;
    //     float zz = za;
    //     xa = xaOrg;
    //     ya = this->footSize;
    //     za = zaOrg;
    //     // TODO: Завкончить обновление move метода
    // }

    this->horizontalCollision = xaOrg != xa || zaOrg != za;
    this->onGround = (yaOrg != ya && yaOrg < 0.0f);
    this->collision = this->horizontalCollision || yaOrg != ya;
    if (this->onGround) {
        if (this->fallDistance > 0.0f) {
            this->causeFallDamage(this->fallDistance);
            this->fallDistance = 0.0f;
        }
    } else if (ya < 0.0f) {
        this->fallDistance -= ya;
    }

    if (xaOrg != xa) {
        this->xd = 0.0f;
    }
    if (yaOrg != ya) {
        this->yd = 0.0f;
    }
    if (zaOrg != za) {
        this->zd = 0.0f;
    }

    this->x = (this->bb.x0 + this->bb.x1) / 2.0f;
    this->y = this->bb.y0 + this->heightOffset;
    this->z = (this->bb.z0 + this->bb.z1) / 2.0f;
    float zz = this->x - xxo;
    xa = this->z - zzo;
    this->walkDist = (float)((float)this->walkDist+std::sqrt((float)(zz*zz+xa*xa))*0.6f);
    if (this->makeStepSound) {
        int tileid = this->level->getTile((int)this->x, (int)(this->y - 0.2f - this->heightOffset), (int)this->z);
        const SoundType* st = Tile::tiles[tileid]->st;
        if (this->walkDist > (float)this->nextStep && tileid > 0) {
            ++this->nextStep;
            // Logger::logf(PREFIX_DEBUG, "STEP! tile: %d, sound: %s\n", tileid, st->name.c_str());
            if (st != &SoundType::none) {
                this->playSound("step." + st->name, st->getVolume()*0.75f, st->getPitch());
            }
        }
    }
    // }
}

bool Entity::isInWater() {
    return level->containsLiquid(bb.grow(0.0f, -0.4f, 0.0f), LiquidType::WATER);
}

bool Entity::isUnderWater() {
    int tileAbove = this->level->getTile((int)this->x, (int)this->y+0.12f, (int)this->z);
    return tileAbove != 0 ? Tile::tiles[tileAbove]->getLiquidType() == LiquidType::WATER : false;
}

bool Entity::isInLava() {
    return level->containsLiquid(bb, LiquidType::LAVA);
}

void Entity::moveRelative(float xa, float za, float speed) {
    float dist =std::sqrt(xa * xa + za * za);
    
    if (dist >= 0.01f) {
        if (dist < 1.0f) {
            dist = 1.0f;
        }
        dist = speed / dist;
        xa *= dist;
        za *= dist;
        
        float yRotRadians = this->yRot * static_cast<float>(M_PI) / 180.0f;
        float sinYRot = std::sin(yRotRadians);
        float cosYRot = std::cos(yRotRadians);
        
        this->xd += xa * cosYRot - za * sinYRot;
        this->zd += za * cosYRot + xa * sinYRot;
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

bool Entity::isPushable() {
    return false;
}

void Entity::push(Entity* e) {
    float dx = e->x - this->x;
    float dz = e->z - this->z;
    float dst = dx*dx + dz*dz;
    if (dst >= 0.01f) {
        dst = std::sqrt(dst);
        dx /= dst;
        dz /= dst;
        dx /= dst;
        dz /= dst;
        dx *= 0.05f;
        dz *= 0.05f;
        dx *= 1.0f - this->pushthrough;
        dz *= 1.0f - this->pushthrough;
        this->push(-dx, 0.0f, -dz);
        e->push(dx, 0.0f, dz);
    }
}

void Entity::push(float x, float y, float z) {
    this->xd += x;
    this->yd += y;
    this->zd += z;
}

void Entity::moveTo(float x, float y, float z, float yRot, float xRot) {
    this->xo = this->x = x;
    this->yo = this->y = y;
    this->zo = this->z = z;
    this->yRot = yRot;
    this->xRot = xRot;
    this->setPos(x, y, z);
}

float Entity::distanceTo(Entity* e) {
    float dx = this->x - e->x;
    float dy = this->y - e->y;
    float dz = this->z - e->z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

float Entity::distanceTo(float x, float y, float z) {
    float dx = this->x - x;
    float dy = this->y - y;
    float dz = this->z - z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

float Entity::distanceToSqr(Entity* e) {
    float dx = this->x - e->x;
    float dy = this->y - e->y;
    float dz = this->z - e->z;
    return dx*dx + dy*dy + dz*dz;
}

bool Entity::isPickable() {
    return false;
}

bool Entity::isShootable() {
    return false;
}

void Entity::causeFallDamage(float fall) {
    // nothing
}

void Entity::playerTouch(Player* player) {
    // nothing
}