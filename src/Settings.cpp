#include "Settings.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <emscripten.h>
#include "util/Logger.hpp"
#include "util/Keyboard.hpp"

Settings::Settings() {
    this->keys = {
        this->key_forward, 
        this->key_left, 
        this->key_back, 
        this->key_right, 
        this->key_jump, 
        this->key_build, 
        this->key_chat, 
        this->key_fog, 
        this->key_save, 
        this->key_load
    };
    this->load();
}

void Settings::load() {
    std::ifstream file(this->path);
    if (!file.is_open()) {
        Logger::logf(PREFIX_CC, "Options file not found, creating default.\n");
        this->save();
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t delimiterPos = line.find(':');
        if (delimiterPos == std::string::npos) continue;

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);
        if (key == "music") this->music = (value == "1");
        else if (key == "sound") this->sound = (value == "1");
        else if (key == "invertYMouse") this->invertYMouse = (value == "1");
        else if (key == "showFPS") this->showFPS = (value == "1");
        else if (key == "renderDistance") this->renderDistance = std::stoi(value);
        else {
            for (auto& binding : this->keys) {
                if (key == "key_" + binding->keyName) {
                    binding->keyCode = std::stoi(value);
                    break;
                }
            }
        }
    }
    file.close();
    Logger::logf(PREFIX_CC, "Options loaded.\n");
}

void Settings::save() {
    std::ofstream file(this->path);
    if (!file.is_open()) {
        Logger::logf(PREFIX_ERROR, "Failed to save options!\n");
        return;
    }

    file << "music:" << (this->music ? "1" : "0") << "\n";
    file << "sound:" << (this->sound ? "1" : "0") << "\n";
    file << "invertYMouse:" << (this->invertYMouse ? "1" : "0") << "\n";
    file << "showFPS:" << (this->showFPS ? "1" : "0") << "\n";
    file << "renderDistance:" << this->renderDistance << "\n";

    for (const auto& binding : this->keys) {
        file << "key_" + binding->keyName << ":" << binding->keyCode << "\n";
    }

    file.close();

    EM_ASM(
        FS.syncfs(false, function (err) {
            if (err) console.error('Error saving config:', err);
        });
    );
}

void Settings::toggleSetting(int id, int d) {
    if (id == 0) {
        this->music = !this->music;
    }

    if (id == 1) {
        this->sound = !this->sound;
    }

    if (id == 2) {
        this->invertYMouse = !this->invertYMouse;
    }

    if (id == 3) {
        this->showFPS = !this->showFPS;
    }

    if (id == 4) {
        this->renderDistance = this->renderDistance + d & 3;
    }

    this->save();
}

std::string Settings::getSettingStateStr(int id) {
    if (id == 0) {
        return ("Music: " + std::string(this->music ? "ON" : "OFF"));
    } else if (id == 1) {
        return ("Sound: " + std::string(this->sound ? "ON" : "OFF"));
    } else if (id == 2) {
        return ("Invert mouse: " + std::string(this->invertYMouse ? "ON" : "OFF"));
    } else if (id == 3) {
        return ("Show FPS: " + std::string(this->showFPS ? "ON" : "OFF"));
    } else if (id == 4) {
        return ("Render distance: " + this->renderVariantsText[this->renderDistance]);
    }
    return "";
}

std::string Settings::getKeyName(int id) {
    return this->keys[id]->keyName + ": " + Keyboard::getKeyName(this->keys[id]->keyCode);
}

void Settings::setKey(int id, int key) {
    this->keys[id]->keyCode = key;
    this->save();
}