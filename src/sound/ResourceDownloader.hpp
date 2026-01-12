#pragma once

#include <string>
#include <functional>
#include <emscripten/fetch.h>
#include <vector>

typedef std::function<void(std::string name, void* data, int size)> OnSoundLoadedCallback;

class ResourceDownloader {
public:
    static void loadSound(const std::string& url, const std::string& name, OnSoundLoadedCallback callback);
};