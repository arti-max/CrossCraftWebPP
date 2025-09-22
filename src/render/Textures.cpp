#include "Textures.hpp"
#include <iostream>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EM_JS(int, js_getImageData, (const char* elementId, unsigned char* buffer, int* width, int* height), {
    const img = document.getElementById(UTF8ToString(elementId));
    if (!img) {
        console.error('Image element not found:', UTF8ToString(elementId));
        return 0;
    }
    
    if (!img.complete || img.naturalWidth === 0) {
        console.error('Image not loaded:', UTF8ToString(elementId));
        return 0;
    }
    
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    canvas.width = img.width;
    canvas.height = img.height;
    ctx.drawImage(img, 0, 0);
    
    const imageData = ctx.getImageData(0, 0, img.width, img.height);
    const data = imageData.data;
    
    HEAP32[width >> 2] = img.width;
    HEAP32[height >> 2] = img.height;
    
    for (let i = 0; i < data.length; i++) {
        HEAP8[buffer + i] = data[i];
    }
    
    console.log('Loaded image:', UTF8ToString(elementId), 'size:', img.width, 'x', img.height);
    return 1;
});

#else
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

GLuint Textures::loadTexture(const std::string& resourceName, int mode) {
    auto it = idMap.find(resourceName);
    if (it != idMap.end()) {
        return it->second;
    }
    
    GLuint textureId = 0;
    
#ifdef __EMSCRIPTEN__
    textureId = loadTextureFromHTMLImage(resourceName, mode);
#else
    textureId = loadTextureFromFile(resourceName, mode);
#endif
    
    if (textureId != 0) {
        idMap[resourceName] = textureId;
    }
    
    return textureId;
}

GLuint Textures::createTextureFromPixels(const unsigned char* pixels, int width, int height, int mode) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
    
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    return id;
}

#ifdef __EMSCRIPTEN__
GLuint Textures::loadTextureFromHTMLImage(const std::string& elementId, int mode) {
    int width, height;
    
    std::vector<unsigned char> buffer(1024 * 1024 * 4);
    
    int success = js_getImageData(elementId.c_str(), buffer.data(), &width, &height);
    if (!success) {
        std::cerr << "Failed to load image: " << elementId << std::endl;
        return 0;
    }
    
    return createTextureFromPixels(buffer.data(), width, height, mode);
}

#else
GLuint Textures::loadTextureFromFile(const std::string& filename, int mode) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;
    }
    
    GLuint textureId = createTextureFromPixels(data, width, height, mode);
    
    stbi_image_free(data);
    return textureId;
}
#endif
