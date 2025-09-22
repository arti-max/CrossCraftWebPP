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
    bool pause = false;
    std::string parent;

    GLFWwindow* window;

    Level* level;
    Player* player;
    Timer* timer = new Timer(20.0f);
    LevelRenderer* levelRenderer;
    Textures* textures;
    HitResult* hitResult;


    // game loop
    void init();
    void tick();
    void render(float partialTicks);
    void pick();
    void destroy();

    // Player camera
    void setupCamera();
    void moveCameraToPlayer(float partialTicks);

    void mainLoop();
    static void emscriptenMainLoop(void* arg);

public:
    CrossCraft(const char* parent, int width, int height, bool fullscreen);
    ~CrossCraft();
    void run();
    void stop();
    void checkGlError(const char str[]);
};