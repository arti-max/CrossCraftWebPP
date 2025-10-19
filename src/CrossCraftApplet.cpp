#include "CrossCraftApplet.hpp"
#include <iostream>
#include <emscripten.h>

static CrossCraftApplet* appletInstance = nullptr;

CrossCraftApplet::CrossCraftApplet() {
    std::cout << "CrossCraftApplet created" << std::endl;
}

CrossCraftApplet::~CrossCraftApplet() {
    destroy();
}

void CrossCraftApplet::setParams(const std::string& user, const std::string& session, 
                               const std::string& mapUser, int mapId, int w, int h) {
    username = user;
    sessionid = session;
    loadMapUser = mapUser;
    loadMapId = mapId;
    width = w;
    height = h;
    
    std::cout << "Applet params set: user=" << username << ", session=" << sessionid 
              << ", size=" << width << "x" << height << std::endl;
    
    if (!username.empty() && !sessionid.empty()) {
        std::cout << "User authenticated: " << username << std::endl;

    } else {
        std::cout << "No authentication provided" << std::endl;
    }
}

void CrossCraftApplet::setServerParams(const std::string& server, int port) {
    serverAddress = server;
    serverPort = port;
    isMultiplayer = true;
    
    std::cout << "Multiplayer params set: server=" << serverAddress 
              << ", port=" << serverPort << std::endl;
}

void CrossCraftApplet::start() {
    std::cout << "=== CrossCraftApplet::start() called ===" << std::endl;
    
    if (game) {
        std::cout << "Game already running" << std::endl;
        return;
    }
    
    try {
        std::cout << "Creating CrossCraft instance..." << std::endl;
        game = new CrossCraft("#canvas", width, height, false);
        std::cout << "CrossCraft instance created" << std::endl;
        
        std::cout << "Setting applet mode..." << std::endl;
        game->appletMode = true;
        std::cout << "Applet mode set" << std::endl;
        
        if (!username.empty() && !sessionid.empty()) {
            std::cout << "Authenticated mode enabled for: " << username << std::endl;
            game->user = new User(username, sessionid);
        }

        if (isMultiplayer && !serverAddress.empty() && serverPort > 0) {
            std::cout << "Multiplayer mode: connecting to " << serverAddress 
                      << ":" << serverPort << std::endl;
            
            std::string wsUrl = "ws://" + serverAddress + ":" + std::to_string(serverPort);
            std::cout << "WebSocket URL: " << wsUrl << std::endl;
            
            game->connectToServer(wsUrl);
            
        } else if (!loadMapUser.empty() && loadMapId != -1) {
            std::cout << "Singleplayer mode: loading map from user " 
                      << loadMapUser << ", id=" << loadMapId << std::endl;
            game->loadMapUser = loadMapUser;
            game->loadMapId = loadMapId;
        }
        
        std::cout << "Calling game->run()..." << std::endl;
        game->run();
        std::cout << "game->run() completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR in CrossCraftApplet::start(): " << e.what() << std::endl;
    }
    
    std::cout << "=== CrossCraftApplet::start() finished ===" << std::endl;
}


void CrossCraftApplet::pause() {
    if (game) {
        game->pause();
    }
}

void CrossCraftApplet::resume() {
    if (game) {
        game->resume();
    }
}

void CrossCraftApplet::destroy() {
    if (game) {
        game->stop();
        delete game;
        game = nullptr;
        std::cout << "CrossCraft applet destroyed" << std::endl;
    }
}

extern "C" {
    void EMSCRIPTEN_KEEPALIVE setAppletParams(const char* username, const char* sessionid, 
                                            const char* loadmapUser, int loadmapId, 
                                            int width, int height) {
        std::string user = username ? username : "";
        std::string session = sessionid ? sessionid : "";
        std::string mapUser = loadmapUser ? loadmapUser : "";
        
        std::cout << "C interface: setAppletParams called" << std::endl;
        std::cout << "  username: " << user << std::endl;
        std::cout << "  sessionid: " << session << std::endl;
        std::cout << "  size: " << width << "x" << height << std::endl;
        
        if (!appletInstance) {
            appletInstance = new CrossCraftApplet();
        }
        
        appletInstance->setParams(user, session, mapUser, loadmapId, width, height);
    }

    void EMSCRIPTEN_KEEPALIVE setServerParams(const char* server, int port) {
        std::cout << "C interface: setServerParams called" << std::endl;
        std::cout << "  server: " << (server ? server : "null") << std::endl;
        std::cout << "  port: " << port << std::endl;
        
        if (!appletInstance) {
            appletInstance = new CrossCraftApplet();
        }
        
        if (server && port > 0) {
            appletInstance->setServerParams(server, port);
        } else {
            std::cout << "Warning: Invalid server parameters" << std::endl;
        }
    }
    
    void EMSCRIPTEN_KEEPALIVE startApplet() {
        std::cout << "C interface: startApplet called" << std::endl;
        if (appletInstance) {
            appletInstance->start();
        } else {
            std::cout << "Error: appletInstance is null!" << std::endl;
        }
    }

    void EMSCRIPTEN_KEEPALIVE testAsyncify() {
        std::cout << "Before sleep" << std::endl;
        emscripten_sleep(1000);
        std::cout << "After sleep" << std::endl;
    }
}
