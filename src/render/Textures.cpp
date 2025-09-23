#include "Textures.hpp"
#include <iostream>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// ИСПРАВЛЕННАЯ EM_JS функция
EM_JS(int, js_getImageData, (const char* elementId, unsigned char* buffer, int bufferSize, int* width, int* height), {
    try {
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
        
        // Проверяем размер буфера
        if (data.length > bufferSize) {
            console.error('Buffer too small for image:', UTF8ToString(elementId), 'need:', data.length, 'have:', bufferSize);
            return 0;
        }
        
        // Записываем размеры
        setValue(width, img.width, 'i32');
        setValue(height, img.height, 'i32');
        
        // Копируем данные
        for (let i = 0; i < data.length; i++) {
            setValue(buffer + i, data[i], 'i8');
        }
        
        console.log('Successfully loaded image:', UTF8ToString(elementId), 'size:', img.width, 'x', img.height);
        return 1;
    } catch (e) {
        console.error('Error in js_getImageData:', e);
        return 0;
    }
});

#else
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

GLuint Textures::loadTexture(const std::string& resourceName, int mode) {
    // std::cout << "Loading texture: " << resourceName << std::endl;
    
    // Проверяем кэш
    auto it = idMap.find(resourceName);
    if (it != idMap.end()) {
        // std::cout << "Texture found in cache: " << it->second << std::endl;
        return it->second;
    }
    
    GLuint textureId = 0;
    
    try {
#ifdef __EMSCRIPTEN__
        textureId = loadTextureFromHTMLImage(resourceName, mode);
#else
        textureId = loadTextureFromFile(resourceName, mode);
#endif
    } catch (const std::exception& e) {
        std::cout << "ERROR loading texture " << resourceName << ": " << e.what() << std::endl;
        return 0;
    }
    
    if (textureId != 0) {
        idMap[resourceName] = textureId;
        // std::cout << "Texture loaded successfully: " << resourceName << " ID: " << textureId << std::endl;
    } else {
        std::cout << "ERROR: Failed to load texture: " << resourceName << std::endl;
    }
    
    return textureId;
}

GLuint Textures::createTextureFromPixels(const unsigned char* pixels, int width, int height, int mode) {
    std::cout << "Creating texture from pixels: " << width << "x" << height << std::endl;
    
    GLuint id;
    glGenTextures(1, &id);
    
    if (id == 0) {
        std::cout << "ERROR: glGenTextures failed" << std::endl;
        return 0;
    }
    
    glBindTexture(GL_TEXTURE_2D, id);
    
    // Проверяем ошибки OpenGL
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL error after glBindTexture: " << error << std::endl;
        return 0;
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
    
    // ИСПОЛЬЗУЕМ glTexImage2D ВМЕСТО gluBuild2DMipmaps (может не работать в gl4es)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL error after glTexImage2D: " << error << std::endl;
        glDeleteTextures(1, &id);
        return 0;
    }
    
    // std::cout << "Texture created successfully with ID: " << id << std::endl;
    return id;
}

#ifdef __EMSCRIPTEN__
GLuint Textures::loadTextureFromHTMLImage(const std::string& elementId, int mode) {
    std::cout << "Loading texture from HTML image: " << elementId << std::endl;
    
    int width, height;
    
    // ДИНАМИЧЕСКИ ВЫДЕЛЯЕМ БУФЕР (не на стеке!)
    const int maxBufferSize = 2048 * 2048 * 4; // Максимум 2K текстура
    std::vector<unsigned char> buffer(maxBufferSize);
    
    int success = js_getImageData(elementId.c_str(), buffer.data(), maxBufferSize, &width, &height);
    if (!success) {
        std::cout << "js_getImageData failed for: " << elementId << std::endl;
        return 0;
    }
    
    std::cout << "Image data retrieved: " << width << "x" << height << std::endl;
    
    // Проверяем размеры
    if (width <= 0 || height <= 0 || width > 2048 || height > 2048) {
        std::cout << "Invalid image dimensions: " << width << "x" << height << std::endl;
        return 0;
    }
    
    return createTextureFromPixels(buffer.data(), width, height, mode);
}

#else
GLuint Textures::loadTextureFromFile(const std::string& filename, int mode) {
    std::cout << "Loading texture from file: " << filename << std::endl;
    
    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4); // Принудительно RGBA
    
    if (!data) {
        std::cout << "stbi_load failed for: " << filename << std::endl;
        return 0;
    }
    
    GLuint textureId = createTextureFromPixels(data, width, height, mode);
    
    stbi_image_free(data);
    return textureId;
}
#endif
