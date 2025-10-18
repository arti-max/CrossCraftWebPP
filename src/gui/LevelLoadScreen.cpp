#include "gui/LevelLoadScreen.hpp"
#include "CrossCraft.hpp"
#include "gui/Button.hpp"
#include <emscripten/fetch.h>
#include <emscripten.h>
#include <sstream>
#include <iostream>

LevelLoadScreen::LevelLoadScreen(Screen* parent) : parent(parent) {}

LevelLoadScreen::~LevelLoadScreen() {
    for (Button* btn : buttons) {
        delete btn;
    }
}

void LevelLoadScreen::init(CrossCraft* cc, int width, int height) {
    Screen::init(cc, width, height);
    this->buttons.clear();
    
    for (int i = 0; i < 5; ++i) {
        Button* btn = new Button(i, this->width / 2 - 100, this->height / 4 + i * 24, 200, 20, "---");
        btn->visible = false;
        this->buttons.push_back(btn);
    }
    
    Button* cancelBtn = new Button(5, this->width / 2 - 100, this->height / 4 + 144, 200, 20, "Cancel");
    this->buttons.push_back(cancelBtn);
    
    loadLevelList();
}

void fetchSuccessCallback(emscripten_fetch_t* fetch) {
    LevelLoadScreen* screen = static_cast<LevelLoadScreen*>(fetch->userData);
    
    std::string text(fetch->data, fetch->numBytes);
    std::cout << "Received response: " << text << std::endl;
    
    screen->levelNames.clear();
    std::stringstream ss(text);
    std::string item;
    
    while (std::getline(ss, item, ';')) {
        screen->levelNames.push_back(item);
    }
    
    if (screen->levelNames.size() < 5) {
        screen->statusMessage = screen->levelNames.empty() ? "Invalid response from server" : screen->levelNames[0];
        screen->loadingFailed = true;
    } else {
        screen->updateButtonLabels(screen->levelNames);
        screen->loadingFinished = true;
    }
    
    emscripten_fetch_close(fetch);
}

void fetchErrorCallback(emscripten_fetch_t* fetch) {
    LevelLoadScreen* screen = static_cast<LevelLoadScreen*>(fetch->userData);
    
    std::cerr << "Failed to load level list. Status: " << fetch->status << std::endl;
    std::cerr << "Status text: " << (fetch->statusText ? fetch->statusText : "null") << std::endl;
    
    screen->statusMessage = "Failed to load levels";
    screen->loadingFailed = true;
    
    emscripten_fetch_close(fetch);
}

void LevelLoadScreen::loadLevelList() {
    statusMessage = "Getting level list..";
    
    std::string url = "http://" + this->cc->serverHost + "/listmaps.jsp?user=" + cc->user->username;
    std::cout << "Fetching: " << url << std::endl;
    
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = fetchSuccessCallback;
    attr.onerror = fetchErrorCallback;
    attr.userData = this;
    
    emscripten_fetch(&attr, url.c_str());
}

void LevelLoadScreen::updateButtonLabels(std::vector<std::string> names) {
    for (int i = 0; i < 5; ++i) {
        Button* btn = this->buttons[i];
        
        btn->enabled = !(names[i] == "-");
        btn->msg = names[i];
        btn->visible = true;
    }
}

void LevelLoadScreen::buttonClicked(Button* btn) {
    if (btn->enabled) {
        if (loadingFinished && btn->id < 5) {
            loadLevel(btn->id);
        }
        
        if (loadingFailed || (loadingFinished && btn->id == 5)) {
            cc->setScreen(parent);
        }
    }
}

void LevelLoadScreen::loadLevel(int levelId) {
    this->cc->loadLevel(this->cc->user->username.c_str(), levelId);
    
    cc->grabMouse();
    cc->setScreen(nullptr);
}

void LevelLoadScreen::render(int xMouse, int yMouse) {
    fillGradient(0, 0, width, height, 537199872, -1607454624);
    drawCenteredString(title.c_str(), width / 2, 40, 0xFFFFFF);
    
    if (!loadingFinished) {
        drawCenteredString(statusMessage.c_str(), width / 2, height / 2 - 4, 0xFFFFFF);
    }
    
    Screen::render(xMouse, yMouse);
}
