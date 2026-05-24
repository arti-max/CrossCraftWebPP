#pragma once
#include "CrossCraft.hpp"
#include <string>
#include <sys/stat.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>

class CrossCraftApplet {
private:
    CrossCraft* game = nullptr;
    CrossCraftApplet();
    CrossCraftApplet(const CrossCraftApplet&) = delete;
    CrossCraftApplet& operator=(const CrossCraftApplet&) = delete;
    static CrossCraftApplet* instance;
    std::string username;
    std::string sessionid;
    std::string loadMapUser;
    int loadMapId = 0;
    int width = 854;
    int height = 480;
    int gamemode = 1;

    std::string serverAddress;
    int serverPort = -1;
    bool isMultiplayer = false;

public:
    static CrossCraftApplet* getInstance();
    ~CrossCraftApplet();
    
    void setParams(const std::string& user, const std::string& session, 
                  const std::string& mapUser, int mapId, int w, int h, int gamemode);
    void setServerParams(const std::string& server, int port);
    void start();
    void pause();
    void resume(); 
    void destroy();

    void initFS();
    
    const std::string& getUsername() const { return username; }
    const std::string& getSessionId() const { return sessionid; }
    bool getIsMultiplayer() const { return isMultiplayer; }
};

extern "C" {
    void EMSCRIPTEN_KEEPALIVE setAppletParams(const char* username, const char* sessionid, 
                                            const char* loadmapUser, int loadmapId, 
                                            int width, int height, int gamemode=1);
    void EMSCRIPTEN_KEEPALIVE startApplet();
    void EMSCRIPTEN_KEEPALIVE setServerParams(const char* server, int port);
}
