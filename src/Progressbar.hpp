#pragma once
#include <string>

class Textures;
class Font;

class Progressbar {
private:
    Textures* textures = nullptr;
    Font* font = nullptr;

    std::string title = "";
    std::string status = "";
    int progress = 0;
    int lastProgress = 0;

    int width = 0;
    int height = 0;
public:
    Progressbar(int width, int height, Textures* textures);

    void startProgressBar(std::string& title);
    void updateProgressStatus(std::string& status);
    void updateProgressState(int progress);
};