#pragma once
#include "gui/Screen.hpp"
#include "gui/Button.hpp"
#include "level/Level.hpp"
#include <vector>
#include <string>

class CrossCraft;
struct emscripten_fetch_t;

void fetchSuccessCallback(emscripten_fetch_t* fetch);
void fetchErrorCallback(emscripten_fetch_t* fetch);

class LevelLoadScreen : public Screen {
private:
    Screen* parent;
    
protected:
    std::string title = "Load level";
    void buttonClicked(Button* btn) override;
    virtual void loadLevel(int levelId);
    
public:
    bool loadingFinished = false;
    bool loadingFailed = false;
    std::vector<std::string> levelNames;
    std::string statusMessage = "";
    
    LevelLoadScreen(Screen* parent);
    ~LevelLoadScreen();
    
    void init(CrossCraft* cc, int width, int height) override;
    void loadLevelList();
    void updateButtonLabels(std::vector<std::string> names);
    void render(int xMouse, int yMouse) override;
    
    friend void fetchSuccessCallback(emscripten_fetch_t* fetch);
    friend void fetchErrorCallback(emscripten_fetch_t* fetch);
};
