#pragma once
#include "gui/Screen.hpp"

class PauseScreen : public Screen {
protected:
    void buttonClicked(Button* btn) override;
public:
    void init() override;
    void render(int xMouse, int yMouse) override;
};