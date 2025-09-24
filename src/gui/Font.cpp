#include "Font.hpp"
#include <GL/gl.h>
#include <iostream>
#include <emscripten.h>
#include <emscripten/html5.h>

Font::Font(const std::string& name, Textures* textures) {
    charWidths.fill(0);
    
    try {
        EM_ASM_({
            var name = UTF8ToString($0);
            var img = document.getElementById(name);
            if (!img || !img.complete || img.naturalWidth === 0) {
                console.error('Font constructor: Image element not found or not loaded:', name);
                throw new Error('Font image not available: ' + name);
            }
            
            var w = img.width;
            var h = img.height;
            var canvas = document.createElement('canvas');
            canvas.width = w;
            canvas.height = h;
            var ctx = canvas.getContext('2d');
            if (!ctx) {
                throw new Error('Could not get 2D context for font processing.');
            }
            
            ctx.drawImage(img, 0, 0);
            var imageData = ctx.getImageData(0, 0, w, h);
            var rawPixels = imageData.data;
            
            Module.fontImageWidth = w;
            Module.fontImageHeight = h;
            Module.fontPixelData = rawPixels;
        }, name.c_str());
        
        int w = EM_ASM_INT({ return Module.fontImageWidth; });
        int h = EM_ASM_INT({ return Module.fontImageHeight; });
        
        for (int i = 0; i < 256; ++i) {
            int col = i % 16;
            int row = i / 16;
            int charPixelWidth = 7;
            
            bool found = false;
            for (; charPixelWidth >= 0 && !found; --charPixelWidth) {
                int x = col * 8 + charPixelWidth;
                for (int y = 0; y < 8; ++y) {
                    int yPixel = row * 8 + y;
                    
                    int alpha = EM_ASM_INT({
                        var pixelIndex = ($0 + $1 * $2) * 4 + 3;
                        return Module.fontPixelData[pixelIndex];
                    }, x, yPixel, w);
                    
                    if (alpha > 128) {
                        found = true;
                        break;
                    }
                }
            }
            
            if (i == 32) {
                charPixelWidth = 3;
            }
            
            charWidths[i] = charPixelWidth + 1;
        }
        
        fontTexture = textures->loadTexture(name, GL_NEAREST);
        
        EM_ASM({
            delete Module.fontImageWidth;
            delete Module.fontImageHeight;
            delete Module.fontPixelData;
        });
        
    } catch (...) {
        std::cerr << "Failed to create font: " << name << std::endl;
        throw;
    }
}

void Font::drawShadow(const std::string& str, float x, float y, int color) {
    internalDraw(str, x + 1, y + 1, color, true);
    internalDraw(str, x, y, color, false);
}

void Font::draw(const std::string& str, float x, float y, int color) {
    internalDraw(str, x, y, color, false);
}

void Font::internalDraw(const std::string& str, float x, float y, int color, bool darken) {
    if (str.empty()) return;
    
    if (darken) {
        color = (color & 0xFCFCFC) >> 2;
    }
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    
    Tessellator& t = Tessellator::getInstance();
    t.begin();
    
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    t.color(r / 255.0f, g / 255.0f, b / 255.0f);
    
    float currentX = x;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '&' && i + 1 < str.length()) {
            std::string colorChars = "0123456789abcdef";
            char colorChar = std::tolower(str[i + 1]);
            size_t colorCode = colorChars.find(colorChar);
            
            if (colorCode != std::string::npos) {
                int iy = (colorCode & 8) * 8;
                int b_c = (colorCode & 1) * 191 + iy;
                int g_c = ((colorCode & 2) >> 1) * 191 + iy;
                int r_c = ((colorCode & 4) >> 2) * 191 + iy;
                
                int newColor = (r_c << 16) | (g_c << 8) | b_c;
                if (darken) {
                    newColor = (newColor & 0xFCFCFC) >> 2;
                }
                
                r = (newColor >> 16) & 0xFF;
                g = (newColor >> 8) & 0xFF;
                b = newColor & 0xFF;
                t.color(r / 255.0f, g / 255.0f, b / 255.0f);
                
                i++;
                continue;
            }
        }
        
        unsigned char charCode = static_cast<unsigned char>(str[i]);
        
        int ix = (charCode % 16) * 8;
        int iy = (charCode / 16) * 8;
        
        float u0 = ix / 128.0f;
        float v0 = iy / 128.0f;
        float u1 = (ix + 7.99f) / 128.0f;
        float v1 = (iy + 7.99f) / 128.0f;
        
        t.vertexUV(currentX, y + 8, 0, u0, v1);
        t.vertexUV(currentX + 8, y + 8, 0, u1, v1);
        t.vertexUV(currentX + 8, y, 0, u1, v0);
        t.vertexUV(currentX, y, 0, u0, v0);
        
        currentX += charWidths[charCode] + CHAR_SPACING;
    }
    
    t.end();
    glDisable(GL_TEXTURE_2D);
}

int Font::width(const std::string& str) {
    if (str.empty()) return 0;
    
    int len = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '&' && i + 1 < str.length()) {
            i++;
        } else {
            unsigned char charCode = static_cast<unsigned char>(str[i]);
            len += charWidths[charCode];
            
            if (i < str.length() - 1) {
                len += CHAR_SPACING;
            }
        }
    }
    return len;
}
