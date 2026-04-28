#pragma once
#include "gui/Screen.hpp"
#include <vector>
#include <string>

class PlayerListScreen : public Screen {
private:
    std::string selectedUsername = "";
public:
    PlayerListScreen();
    void render(int xMouse, int yMouse) override;
protected:
    void mouseClicked(int x, int y, int mbtn) override;
};
