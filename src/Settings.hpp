#pragma once
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include "Key.hpp"

class Settings {
private:
    std::string path = "/.crosscraft/options.txt";
    std::vector<std::string> renderVariantsText = {"FAR", "NORMAL", "SHORT", "TINY"};
public:
    bool music = true;
    bool sound = true;
    bool invertYMouse = false;
    bool showFPS = false;
    int renderDistance = 0;

    KeyMap* key_forward = new KeyMap("Forward", GLFW_KEY_W);
    KeyMap* key_left = new KeyMap("Left", GLFW_KEY_A);
    KeyMap* key_back = new KeyMap("Back", GLFW_KEY_S);
    KeyMap* key_right = new KeyMap("Right", GLFW_KEY_D);
    KeyMap* key_jump = new KeyMap("Jump", GLFW_KEY_SPACE);
    KeyMap* key_build = new KeyMap("Build", GLFW_KEY_B);
    KeyMap* key_chat = new KeyMap("Chat", GLFW_KEY_T);
    KeyMap* key_fog = new KeyMap("Toggle Fog", GLFW_KEY_F);
    KeyMap* key_save = new KeyMap("Save location", GLFW_KEY_ENTER);
    KeyMap* key_load = new KeyMap("Load location", GLFW_KEY_R);

    std::vector<KeyMap*> keys;

    Settings();
    void load();
    void save();

    std::string getKeyName(int i);
    std::string getSettingStateStr(int i);
    void setKey(int i, int key);
    void toggleSetting(int i, int d);

};