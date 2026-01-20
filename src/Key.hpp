#pragma once
#include <string>

class KeyMap {
public:
    int keyCode;
    std::string keyName;

    KeyMap(std::string name, int key) {
        this->keyName = name;
        this->keyCode = key;
    }
};