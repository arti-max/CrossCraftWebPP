#pragma once
#include "net/packet/Packet.hpp"
#include "net/packet/LoginPacket.hpp"
#include "net/packet/BlockChangePacket.hpp"
#include <emscripten/websocket.h>
#include <functional>
#include <queue>
#include <string>

class Client {
private:
    EMSCRIPTEN_WEBSOCKET_T socket = 0;
    std::string serverUrl;
    bool connected = false;
    std::queue<Packet*> outgoingPackets;
    
    std::function<void()> onConnectCallback;
    std::function<void(Packet*)> onPacketCallback;
    std::function<void(const std::string&)> onErrorCallback;

    static EM_BOOL onOpen(int eventType, const EmscriptenWebSocketOpenEvent* event, void* userData);
    static EM_BOOL onError(int eventType, const EmscriptenWebSocketErrorEvent* event, void* userData);
    static EM_BOOL onClose(int eventType, const EmscriptenWebSocketCloseEvent* event, void* userData);
    static EM_BOOL onMessage(int eventType, const EmscriptenWebSocketMessageEvent* event, void* userData);

public:
    Client();
    ~Client();

    bool connect(const std::string& url);
    void disconnect();
    bool isConnected() const { return connected; }

    void sendPacket(Packet* packet);
    void tick(); 

    void setOnConnect(std::function<void()> callback) { onConnectCallback = callback; }
    void setOnPacket(std::function<void(Packet*)> callback) { onPacketCallback = callback; }
    void setOnError(std::function<void(const std::string&)> callback) { onErrorCallback = callback; }
};
