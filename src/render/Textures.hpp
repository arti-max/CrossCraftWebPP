#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

class Textures {
private:
    std::unordered_map<std::string, GLuint> idMap;

public:
    GLuint loadTexture(const std::string& resourceName, int mode);
    
private:
    GLuint createTextureFromPixels(const unsigned char* pixels, int width, int height, int mode);
    
#ifdef __EMSCRIPTEN__
    GLuint loadTextureFromHTMLImage(const std::string& elementId, int mode);
#else
    GLuint loadTextureFromFile(const std::string& filename, int mode);
#endif
};
