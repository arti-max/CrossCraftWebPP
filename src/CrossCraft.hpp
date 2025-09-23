#pragma once
#include <emscripten/html5.h>
#include <GLFW/glfw3.h>
#include <string>
#include <array>
#include "util/Mouse.hpp"
#include "util/Keyboard.hpp"
#include "level/Level.hpp"
#include "level/render/LevelRenderer.hpp"
#include "Player.hpp"
#include "Timer.hpp"
#include "render/Textures.hpp"
#include "HitResult.hpp"

class CrossCraft {
private:
    int lastFpsTime = 0;
    int frames = 0;
    std::string fpsString;

    std::array<float, 4> fogColor0;
    std::array<float, 4> fogColor1;
    int width, height;
    bool fullscreen;
    bool running = false;
    bool paused = false;
    std::string parent;

    GLFWwindow* window;

    Level* level;
    Player* player;
    Timer* timer = new Timer(20.0f);
    LevelRenderer* levelRenderer;
    Textures* textures = nullptr;
    HitResult* hitResult;

    int yMouseAxis = -1;

    // game loop
    void init();
    void tick();
    void render(float partialTicks);
    void pick();
    void destroy();

    // Player camera
    void setupCamera(float partialTicks);
    void moveCameraToPlayer(float partialTicks);
    void setupFog(int layer);
    inline const float* getBuffer(float a, float b, float c, float d) {
        static float buf[4];
        buf[0] = a; buf[1] = b; buf[2] = c; buf[3] = d;
        return buf;
    }

    void mainLoop();
    static void emscriptenMainLoop(void* arg);

public:
    CrossCraft(const char* parent, int width, int height, bool fullscreen);
    ~CrossCraft();

    bool appletMode = false;

    void run();
    void stop();
    void pause();
    void resume();
    void checkGlError(const char str[]);
};