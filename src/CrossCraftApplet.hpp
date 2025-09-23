#pragma once
#include "CrossCraft.hpp"
#include <string>

class CrossCraftApplet {
private:
    CrossCraft* game = nullptr;
    std::string username;
    std::string sessionid;
    std::string loadMapUser;
    int loadMapId = 0;
    int width = 854;
    int height = 480;

public:
    CrossCraftApplet();
    ~CrossCraftApplet();
    
    void setParams(const std::string& user, const std::string& session, 
                  const std::string& mapUser, int mapId, int w, int h);
    void start();
    void pause();
    void resume(); 
    void destroy();
    
    const std::string& getUsername() const { return username; }
    const std::string& getSessionId() const { return sessionid; }
};

extern "C" {
    void EMSCRIPTEN_KEEPALIVE setAppletParams(const char* username, const char* sessionid, 
                                            const char* loadmapUser, int loadmapId, 
                                            int width, int height);
    void EMSCRIPTEN_KEEPALIVE startApplet();
}
