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
    xo = x;
    yo = y;
    zo = z;
    
    float forward = 0.0f;
    float vertical = 0.0f;
    
    if (keys[KEY_UP]) {
        --forward;
    }
    if (keys[KEY_DOWN]) {
        ++forward;
    }
    if (keys[KEY_LEFT]) {
        --vertical;
    }
    if (keys[KEY_RIGHT]) {
        ++vertical;
    }
    
    if (keys[KEY_JUMP]) {
        if (onGround) {
            yd = 0.5f;
        }
    }
    
    moveRelative(vertical, forward, onGround ? 0.1f : 0.02f);

    yd -= 0.08f;

    move(xd, yd, zd);
    
    xd *= 0.91f;
    yd *= 0.98f;
    zd *= 0.91f;
    
    if (onGround) {
        xd *= 0.6f;
        zd *= 0.6f;
    }
}
