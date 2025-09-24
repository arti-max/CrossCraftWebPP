#pragma once
#include <array>
#include <string>
#include <vector>
#include "render/Tessellator.hpp"
#include "render/Textures.hpp"

class Font {
private:
    static const int CHAR_SPACING = 2;
    std::array<int, 256> charWidths;
    int fontTexture = 0;
    
    void internalDraw(const std::string& str, float x, float y, int color, bool darken);

public:
    Font(const std::string& name, Textures* textures);
    ~Font() = default;
    
    void drawShadow(const std::string& str, float x, float y, int color);
    void draw(const std::string& str, float x, float y, int color);
    int width(const std::string& str);
};
