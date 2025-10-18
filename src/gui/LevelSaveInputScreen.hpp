#pragma once
#include "gui/Screen.hpp"
#include "util/Keyboard.hpp"
#include <string>

class LevelSaveInputScreen : public Screen {
private:
    Screen* parent;
    std::string title = "Enter level name:";
    int levelId;
    std::string levelName;
    int tickCounter = 0;
public:
    LevelSaveInputScreen(Screen* parent, std::string currentName, int levelId);
    ~LevelSaveInputScreen();

    void init(CrossCraft* cc, int width, int height) override;
    void onClose() override;
    void tick() override;
protected:
    void buttonClicked(Button* btn) override;
    void keyPressed(char c, int key) override;
    void render(int xMouse, int yMouse) override;
};