#pragma once
#include "gui/Screen.hpp"
#include <vector>
#include <string>

class PlayerListScreen : public Screen {
public:
    PlayerListScreen();
    void render(int xMouse, int yMouse) override;
};
