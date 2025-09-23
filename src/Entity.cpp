#include "Entity.hpp"
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
    float x = randomFloat() * static_cast<float>(level->width - 2) + 1.0f;
    float y = static_cast<float>(level->depth + 10);
    float z = randomFloat() * static_cast<float>(level->height - 2) + 1.0f;
    setPos(x, y, z);
}

void Entity::remove() {
    removed = true;
}

void Entity::setSize(float w, float h) {
    bbWidth = w;
    bbHeight = h;
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

void Entity::tick() {
    xo = x;
    yo = y;
    zo = z;
}

bool Entity::isFree(float xa, float ya, float za) {
    AABB box = bb.cloneMove(xa, ya, za);
    std::vector<AABB> aabbs = level->getCubes(box);
    
    if (aabbs.size() > 0) {
        return false;
    }
    
    return !level->containsAnyLiquid(box);
}

void Entity::move(float xa, float ya, float za) {
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

    horizontalCollision = (xaOrg != xa || zaOrg != za);
    onGround = (yaOrg != ya && yaOrg < 0.0f);

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

void Entity::render(float a) {
}

float Entity::randomFloat() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(randomGenerator);
}
