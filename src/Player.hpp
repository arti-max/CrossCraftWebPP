#pragma once
#include "Entity.hpp"

class Player : public Entity {
public:
    static const int KEY_UP = 0;
    static const int KEY_DOWN = 1;
    static const int KEY_LEFT = 2;
    static const int KEY_RIGHT = 3;
    static const int KEY_JUMP = 4;

private:
    bool keys[10] = {false};

public:
    Player(Level* level);
    
    void setKey(int key, bool state);
    void releaseAllKeys();
    void tick() override;
};
