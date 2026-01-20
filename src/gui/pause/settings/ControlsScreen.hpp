#pragma once
#include "gui/Screen.hpp"
#include "Settings.hpp"

class ControlsScreen : public Screen {
private:
    Screen* parent;
    std::string title = "Controls";
    Settings* settings;
    int key = -1;
protected:
    void buttonClicked(Button* btn) override;
public:
    ControlsScreen(Screen* parent, Settings* settings);
    void render(int xMouse, int yMouse) override;
    void keyPressed(char eventCharacter, int eventKey) override;
    void init() override;
};