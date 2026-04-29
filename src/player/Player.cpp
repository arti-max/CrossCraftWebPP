#include "player/Player.hpp"
#include "util/Keyboard.hpp"
#include "util/Logger.hpp"
#include "level/Level.hpp"
#include <GLFW/glfw3.h>

Player::Player(Level* level, Settings* settings) : Entity(level) {
    this->heightOffset = 1.62f;
    this->settings = settings;
}

void Player::setKey() {
    keys[KEY_UP] = Keyboard::isKeyDown(this->settings->key_forward->keyCode);
    keys[KEY_DOWN] = Keyboard::isKeyDown(this->settings->key_back->keyCode);
    keys[KEY_LEFT] = Keyboard::isKeyDown(this->settings->key_left->keyCode);
    keys[KEY_RIGHT] = Keyboard::isKeyDown(this->settings->key_right->keyCode);
    keys[KEY_JUMP] = Keyboard::isKeyDown(this->settings->key_jump->keyCode);
}

void Player::releaseAllKeys() {
    for (int i = 0; i < 10; ++i) {
        keys[i] = false;
    }
}

// void Player::resetPos() {
//     this->heightOffset = 1.62;
//     this->setSize(0.6f, 1.8f);
//     if (this->level != nullptr) {
//         this->level->player = this;
//     }
// }

void Player::tick() {
    this->xo = x;
    this->yo = y;
    this->zo = z;
    
    float xa = 0.0f;
    float ya = 0.0f;
    bool inWater = this->isInWater();
    bool inLava = this->isInLava();

    if (keys[KEY_UP]) {
        --ya;
    }
    if (keys[KEY_DOWN]) {
        ++ya;
    }
    if (keys[KEY_LEFT]) {
        --xa;
    }
    if (keys[KEY_RIGHT]) {
        ++xa;
    }
    
    if (keys[KEY_JUMP]) {
        if (inWater) {
            this->yd += 0.04f;
        } else if (inLava) {
            this->yd += 0.04f;
        } else if (onGround) {
            this->yd = 0.42f;
        }
    }
    
    float oldY;
    if (inWater) {
        oldY = this->y;
        this->moveRelative(xa, ya, 0.02f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.8f;
        this->yd *= 0.8f;
        this->zd *= 0.8f;
        this->yd -= 0.02f;
        // Logger::logf(PREFIX_DEBUG, "hor: %i, is free: %i", this->horizontalCollision, this->isFree(this->xd, this->yd + 0.6f - this->y + oldY, this->zd));
        if (this->horizontalCollision && this->isFree(this->xd, this->yd + 0.6f - this->y + oldY, this->zd)) {
            this->yd = 0.3f;
        }
    } else if (inLava) {
        oldY = this->y;
        this->moveRelative(xa, ya, 0.02f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.5f;
        this->yd *= 0.5f;
        this->zd *= 0.5f;
        this->yd -= 0.02f;
        
        if (this->horizontalCollision && this->isFree(this->xd, this->yd + 0.6f - this->y + oldY, this->zd)) {
            this->yd = 0.3f;
        }
    } else {
        this->moveRelative(xa, ya, this->onGround ? 0.1f : 0.02f);
        this->move(this->xd, this->yd, this->zd);
        
        this->xd *= 0.91f;
        this->yd *= 0.98f;
        this->zd *= 0.91f;
        this->yd -= 0.08f;
        
        if (onGround) {
            this->xd *= 0.6f;
            this->zd *= 0.6f;
        }
    }

    std::vector<Entity*> entities = this->level->findEntities(this, this->bb.grow(1.0f, 0.0f, 1.0f));
    if (entities.size() > 0) {
        for (int i = 0; i < entities.size(); ++i) {
            entities[i]->playerTouch(this);
        }
    }

}

bool Player::isPlayer() {
    return true;
}