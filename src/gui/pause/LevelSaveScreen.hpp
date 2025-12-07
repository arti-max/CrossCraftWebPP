#pragma once
#include "gui/pause/LevelLoadScreen.hpp"
#include "gui/pause/LevelSaveInputScreen.hpp"
#include "gui/Screen.hpp"
#include <vector>
#include <string>

class LevelSaveScreen : public LevelLoadScreen {
protected:
    void updateButtonLabels(std::vector<std::string> names) override;
    void loadLevel(int levelId) override;
public:
    LevelSaveScreen(Screen* parent);
    ~LevelSaveScreen();
};