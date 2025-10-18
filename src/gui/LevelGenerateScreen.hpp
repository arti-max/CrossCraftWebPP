#pragma once
#include "gui/Screen.hpp"
#include <string>
#include <vector>

class LevelGenerateScreen : public Screen {
private:
    Screen* parent;
    std::string title = "Generate new level";
    std::vector<std::string> buttonNames;
protected:
    void buttonClicked(Button* btn) override;
public:
    LevelGenerateScreen(Screen* parent);
    void init() override;
    void render(int xMouse, int yMouse) override;
};