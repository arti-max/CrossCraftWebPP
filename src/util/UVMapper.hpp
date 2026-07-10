#include <iostream>

struct UVCoords {
    float u0, v0, u1, v1;
};

namespace UVMapper {

    inline UVCoords map(float x, float y, float spriteWidth, float spriteHeight, float atlasWidth, float atlasHeight) {
        float w = (atlasWidth > 0.0f) ? atlasWidth : 1.0f;
        float h = (atlasHeight > 0.0f) ? atlasHeight : 1.0f;

        return {
            x / w,
            y / h,
            (x + spriteWidth) / w,
            (y + spriteHeight) / h
        };
    }
}