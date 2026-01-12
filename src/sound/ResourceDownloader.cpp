#include "sound/ResourceDownloader.hpp"
#include <iostream>
#include <cstring>

struct DownloadContext {
    std::string name;
    OnSoundLoadedCallback callback;
};

void downloadSucceeded(emscripten_fetch_t *fetch) {
    DownloadContext* ctx = static_cast<DownloadContext*>(fetch->userData);
    
    if (ctx && ctx->callback) {
        void* dataCopy = malloc(fetch->numBytes);
        memcpy(dataCopy, fetch->data, fetch->numBytes);
        
        ctx->callback(ctx->name, dataCopy, fetch->numBytes);
    }
    
    delete ctx;
    emscripten_fetch_close(fetch);
}

void downloadFailed(emscripten_fetch_t *fetch) {
    DownloadContext* ctx = static_cast<DownloadContext*>(fetch->userData);
    std::cerr << "ResourceDownloader: Failed to download " << ctx->name 
              << " HTTP Status: " << fetch->status << std::endl;
    
    delete ctx;
    emscripten_fetch_close(fetch);
}

void ResourceDownloader::loadSound(const std::string& url, const std::string& name, OnSoundLoadedCallback callback) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = downloadSucceeded;
    attr.onerror = downloadFailed;
    DownloadContext* ctx = new DownloadContext();
    ctx->name = name;
    ctx->callback = callback;
    
    attr.userData = ctx;
    
    emscripten_fetch(&attr, url.c_str());
}
