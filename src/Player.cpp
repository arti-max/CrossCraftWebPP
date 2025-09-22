#include "Player.hpp"

Player::Player(Level* level) : Entity(level) {
    heightOffset = 1.62f;
}

void Player::setKey(int key, bool state) {
    int id = -1;
    
    if (key == 200 || key == 17) {
        id = KEY_UP;
    }
    else if (key == 208 || key == 31) {
        id = KEY_DOWN;
    }
    else if (key == 203 || key == 30) {
        id = KEY_LEFT;
    }
    else if (key == 205 || key == 32) {
        id = KEY_RIGHT;
    }
    else if (key == 57 || key == 219) {
        id = KEY_JUMP;
    }
    
    if (id >= 0) {
        keys[id] = state;
    }
}

void Player::releaseAllKeys() {
    for (int i = 0; i < 10; ++i) {
        keys[i] = false;
    }
}

void Player::tick() {
    xo = x;
    yo = y;
    zo = z;
    
    float xa = 0.0f;
    float ya = 0.0f;
    
    bool inWater = isInWater();
    bool inLava = isInLava();
    
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
            yd += 0.04f;
        } else if (inLava) {
            yd += 0.04f;
        } else if (onGround) {
            yd = 0.42f;
            keys[KEY_JUMP] = false;
        }
    }
    
    if (inWater) {
        float yo = y;
        moveRelative(xa, ya, 0.02f);
        move(xd, yd, zd);
        
        xd *= 0.8f;
        yd *= 0.8f;
        zd *= 0.8f;
        yd -= 0.02f;
        
        if (horizontalCollision && isFree(xd, yd + 0.6f - y + yo, zd)) {
            yd = 0.3f;
        }
    } 
    else if (inLava) {
        float yo = y;
        moveRelative(xa, ya, 0.02f);
        move(xd, yd, zd);
        
        xd *= 0.5f;
        yd *= 0.5f;
        zd *= 0.5f;
        yd -= 0.02f;
        
        if (horizontalCollision && isFree(xd, yd + 0.6f - y + yo, zd)) {
            yd = 0.3f;
        }
    } 
    else {
        moveRelative(xa, ya, onGround ? 0.1f : 0.02f);
        move(xd, yd, zd);
        
        xd *= 0.91f;
        yd *= 0.98f;
        zd *= 0.91f;
        yd -= 0.08f;
        
        if (onGround) {
            xd *= 0.6f;
            zd *= 0.6f;
        }
    }
}
