#pragma once
#include <emscripten/html5.h>
#include <emscripten.h>
#include <GLFW/glfw3.h>
#include <string>
#include <array>
#include "util/Mouse.hpp"
#include "util/Keyboard.hpp"
#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include "level/render/LevelRenderer.hpp"
#include "player/Player.hpp"
#include "Timer.hpp"
#include "render/Textures.hpp"
#include "HitResult.hpp"
#include "gui/Font.hpp"
#include "util/Ray.hpp"
#include "Entity.hpp"
#include "character/Zombie.hpp"
#include "gui/Screen.hpp"
#include "gui/pause/PauseScreen.hpp"
#include "gui/ingame/ChatScreen.hpp"
#include "gui/ingame/ChatGui.hpp"
#include "gui/ingame/PlayerListScreen.hpp"
#include "gui/ingame/Hud.hpp"
#include "gui/ingame/BlockSelectScreen.hpp"
#include "User.hpp"
#include "util/Logger.hpp"
#include "level/LevelLoaderListener.hpp"
#include "level/levelgen/LevelGen.hpp"
#include "level/LevelIO.hpp"
#include "particle/ParticleEngine.hpp"
#include "net/Client.hpp"
#include "net/NetworkPlayer.hpp"
#include "render/fx/TextureFX.hpp"
#include "render/fx/WaterTextureFX.hpp"
#include "render/fx/LavaTextureFX.hpp"
#include "item/Item.hpp"
#include "sound/SoundManager.hpp"
#include "sound/SoundType.hpp"
#include "Settings.hpp"
#include "Data.hpp"

class CrossCraft : public LevelLoaderListener {
private:
    const std::string VERSION_STRING = "0.0.13a_01";
    int lastFpsTime = 0;
    int frames = 0;
    int lastProgress = -1;
    std::string fpsString;
    std::string title = "";
    std::string status = "";
    bool inErrorState = false;
    std::string errorTitle;
    std::string errorReason;
    std::vector<std::string> connectionUrls;
    int currentUrlIndex = 0;
    int playerId = -1;
    
    std::array<float, 4> fogColor0;
    std::array<float, 4> fogColor1;
    std::array<int, 16> viewportBuffer;
    std::array<int, 2000> selectBuffer;
    std::vector<TextureFX*> textureEffects;
    bool fullscreen;
    bool running = false;
    bool paused = false;
    bool canRender = true;
    std::string parent;
    int editMode = 0;
    int selectedTile = Tile::rock->id;
    bool mouseGrabbed = false;
    float fogDistance = 0.0f;
    int attackTime = 0;
    int clickDelay = 0;
    bool isDrop = false;


    float lastSentX = 0, lastSentY = 0, lastSentZ = 0;
    float lastSentYaw = 0, lastSentPitch = 0;

    Timer* timer = new Timer(20.0f);
    LevelRenderer* levelRenderer;
    HitResult* hitResult;
    LevelGen* levelGen = new LevelGen(this);
    LevelIO* levelIO = new LevelIO(this);
    ParticleEngine* particleEngine;
    Hud* hud;
    BlockSelectScreen* blockSelectScreen;
    
    std::vector<int> hotbarSlots;
    int hotbarIndex = 0;
    int yMouseAxis = -1;

    float bgR = 0.0f;
    float bgG = 0.0f;
    float bgB = 0.0f;

    // game loop
    void init();
    void tick();
    void render(float partialTicks);
    void raycast();
    void destroy();
    void handleMouseClick();
    bool isFree(const AABB &aabb);
    void fill(int x0, int y0, int x1, int y1, int col);

    int getSelectedTile();

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
    void asyncSleep(int ms);

public:
    static CrossCraft* instance;
    int width, height;
    GLFWwindow* window;

    Level* level;
    Screen* screen = nullptr;
    Font* font;
    Textures* textures = nullptr;
    Player* player;
    Client* client = nullptr;
    ChatGui* chatGui;
    PlayerListScreen* playerListScreen = new PlayerListScreen();
    SoundManager* sound;
    Settings* settings = new Settings();

    CrossCraft(const char* parent, int width, int height, bool fullscreen);
    ~CrossCraft();

    bool appletMode = false;
    bool mpMode = false;
    bool waitingForFocus = false;

    int serverPort;
    std::string serverAddress;

    Data* userData = nullptr;
    int loadMapId = 0;
    std::string loadMapUser = "";
    std::string serverHost = "crosscraftweb.ddns.net";

    void run();
    void stop();
    void pause();
    void resume();
    void checkGlError(const char str[]);
    void setScreen(Screen* screen);
    void grabMouse();
    void releaseMouse();
    void drawErrorScreen();

    void generateNewLevel(int width, int height, int depth);
    bool loadLevel(const char username[], int levelId);
    void saveLevel(int levelId, const char levelname[]);
    void beginLevelLoading(const char str[]) override;
    void levelLoadUpdate(const char str[]) override;
    void levelLoadProgress(int progress) override;
    void showErrorScreen(const std::string& title, const std::string& reason);

    void connectToServer();
    void handleNetworkPacket(Packet* packet);
    void connectError(std::string error);
};

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void showCrashScreen(const char* msg);
}