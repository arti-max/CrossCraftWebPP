#pragma once
#include <emscripten/html5.h>
#include <GLFW/glfw3.h>
#include <string>
#include <array>
#include "util/Mouse.hpp"
#include "util/Keyboard.hpp"
#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include "level/render/LevelRenderer.hpp"
#include "Player.hpp"
#include "Timer.hpp"
#include "render/Textures.hpp"
#include "HitResult.hpp"
#include "gui/Font.hpp"
#include "util/Ray.hpp"

class CrossCraft {
private:
    const std::string VERSION_STRING = "0.0.3a";
    int lastFpsTime = 0;
    int frames = 0;
    std::string fpsString;

    std::array<float, 4> fogColor0;
    std::array<float, 4> fogColor1;
    std::array<int, 16> viewportBuffer;
    std::array<int, 2000> selectBuffer;
    int width, height;
    bool fullscreen;
    bool running = false;
    bool paused = false;
    std::string parent;
    int editMode = 0;
    int selectedTile = Tile::rock->id;

    GLFWwindow* window;

    Level* level;
    Player* player;
    Timer* timer = new Timer(20.0f);
    LevelRenderer* levelRenderer;
    Textures* textures = nullptr;
    Font* font;
    HitResult* hitResult;

    int yMouseAxis = -1;

    // game loop
    void init();
    void tick();
    void render(float partialTicks);
    void raycast(float partialTicks);
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
    void drawGui(float partialTicks);

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