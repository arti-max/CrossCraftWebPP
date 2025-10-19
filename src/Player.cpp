#include "Player.hpp"
#include "util/Keyboard.hpp"
#include <GLFW/glfw3.h>

Player::Player(Level* level) : Entity(level) {
    this->heightOffset = 1.62f;
}

void Player::setKey() {
    keys[KEY_UP] = Keyboard::isKeyDown(GLFW_KEY_W);
    keys[KEY_DOWN] = Keyboard::isKeyDown(GLFW_KEY_S);
    keys[KEY_LEFT] = Keyboard::isKeyDown(GLFW_KEY_A);
    keys[KEY_RIGHT] = Keyboard::isKeyDown(GLFW_KEY_D);
    keys[KEY_JUMP] = Keyboard::isKeyDown(GLFW_KEY_SPACE);
}

void Player::releaseAllKeys() {
    for (int i = 0; i < 10; ++i) {
        keys[i] = false;
    }
}

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
    
    if (inWater) {
        this->moveRelative(xa, ya, 0.02f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.8f;
        this->yd *= 0.8f;
        this->zd *= 0.8f;
        this->yd -= 0.02f;
        if (this->horizontalCollision) {
            int px = (int)(this->x);
            int py = (int)(this->y + 1.8f);
            int pz = (int)(this->z);
            bool blockAbove = level->isSolidTile(px, py, pz);
            if (!blockAbove) {
                this->yd = 0.3f;
            }
        }
    } else if (inLava) {
        this->moveRelative(xa, ya, 0.02f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.5f;
        this->yd *= 0.5f;
        this->zd *= 0.5f;
        this->yd -= 0.02f;
        
        if (this->horizontalCollision) {
            int px = (int)(this->x);
            int py = (int)(this->y + 1.8f);
            int pz = (int)(this->z);
            
            if (!level->isSolidTile(px, py, pz)) {
                this->yd = 0.3f;
            }
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
}
