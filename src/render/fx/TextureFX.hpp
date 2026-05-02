#pragma once
#include <GL/gl.h>

class TextureFX {
public:
    unsigned char pixels[16 * 16 * 4];
    int textureId;
    bool anaglyph = false;

    explicit TextureFX(int textureId) : textureId(textureId) {}

    virtual ~TextureFX() = default;

    virtual void tick() = 0;
};