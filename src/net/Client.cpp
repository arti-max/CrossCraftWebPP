#include "net/Client.hpp"
#include "util/Logger.hpp"

Client::Client() {}

Client::~Client() {
    disconnect();
}

bool Client::connect(const std::string& url) {
    if (connected) {
        Logger::logf(PREFIX_ERROR, "Already connected to server\n");
        return false;
    }

    serverUrl = url;

    EmscriptenWebSocketCreateAttributes attrs;
    emscripten_websocket_init_create_attributes(&attrs);
    attrs.url = url.c_str();

    socket = emscripten_websocket_new(&attrs);
    
    if (socket <= 0) {
        Logger::logf(PREFIX_ERROR, "Failed to create WebSocket\n");
        return false;
    }

    emscripten_websocket_set_onopen_callback(socket, this, onOpen);
    emscripten_websocket_set_onerror_callback(socket, this, onError);
    emscripten_websocket_set_onclose_callback(socket, this, onClose);
    emscripten_websocket_set_onmessage_callback(socket, this, onMessage);

    Logger::logf(PREFIX_NETWORK, "Connecting to %s...\n", url.c_str());
    return true;
}

void Client::disconnect() {
    if (socket > 0) {
        emscripten_websocket_close(socket, 1000, "Client disconnect");
        emscripten_websocket_delete(socket);
        socket = 0;
    }
    connected = false;
}

void Client::sendPacket(Packet* packet) {
    if (!connected) {
        Logger::logf(PREFIX_WARNING, "Cannot send packet: not connected\n");
        delete packet;
        return;
    }

    packet->serialize();
    std::vector<uint8_t> bytes = packet->toBytes();
    
    emscripten_websocket_send_binary(socket, bytes.data(), bytes.size());
    
    delete packet;
}

void Client::tick() {
    
}

EM_BOOL Client::onOpen(int eventType, const EmscriptenWebSocketOpenEvent* event, void* userData) {
    Client* client = static_cast<Client*>(userData);
    client->connected = true;
    Logger::logf(PREFIX_NETWORK, "Connected to server!\n");
    
    if (client->onConnectCallback) {
        client->onConnectCallback();
    }
    
    return EM_TRUE;
}

EM_BOOL Client::onError(int eventType, const EmscriptenWebSocketErrorEvent* event, void* userData) {
    Client* client = static_cast<Client*>(userData);
    Logger::logf(PREFIX_ERROR, "WebSocket error\n");
    
    if (client->onErrorCallback) {
        client->onErrorCallback("WebSocket error");
    }
    
    return EM_TRUE;
}

EM_BOOL Client::onClose(int eventType, const EmscriptenWebSocketCloseEvent* event, void* userData) {
    Client* client = static_cast<Client*>(userData);
    client->connected = false;
    Logger::logf(PREFIX_NETWORK, "Disconnected from server (code: %d)\n", event->code);
    
    return EM_TRUE;
}

EM_BOOL Client::onMessage(int eventType, const EmscriptenWebSocketMessageEvent* event, void* userData) {
    Client* client = static_cast<Client*>(userData);
    
    if (event->isText) {
        Logger::logf(PREFIX_WARNING, "Received text message (expected binary)\n");
        return EM_TRUE;
    }
    
    Packet* packet = Packet::fromBytes(event->data, event->numBytes);
    
    if (packet && client->onPacketCallback) {
        client->onPacketCallback(packet);
    }
    
    delete packet;
    
    return EM_TRUE;
}
