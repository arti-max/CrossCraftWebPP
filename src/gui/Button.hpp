#pragma once
#include <string>

class Button {
public:
    int id = 0;
    int x = 0;
    int y = 0; 
    int w = 0;
    int h = 0;
    std::string msg = "";
    bool visible = true;
    bool enabled = true;

    Button(int id, int x, int y, std::string msg) {
        this->x = x;
        this->id = id;
        this->y = y;
        this->w = 200;
        this->h = 20;
        this->msg = std::move(msg);
    }

    Button(int id, int x, int y, int w, int h, std::string msg) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->msg = std::move(msg);
    }
};