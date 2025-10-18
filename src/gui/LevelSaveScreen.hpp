#pragma once
#include "gui/LevelLoadScreen.hpp"
#include "gui/LevelSaveInputScreen.hpp"
#include "gui/Screen.hpp"
#include <vector>
#include <string>

class LevelSaveScreen : public LevelLoadScreen {
protected:
    void updateButtonLabels(std::vector<std::string> names);
    void loadLevel(int levelId) override;
public:
    LevelSaveScreen(Screen* parent);
    ~LevelSaveScreen();
};