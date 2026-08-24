#pragma once
#include "level/Level.hpp"
#include "mob/AnimalMob.hpp"
#include "mob/Creeper.hpp"
#include "mob/Spider.hpp"
#include "util/Logger.hpp"
#include "level/io/consts.hpp"

class LevelIO;

class v4io {
private:
    const int IO_VERSION = 4;
    LevelIO* levelIO = nullptr;
public:
    void setIO(LevelIO* io) {
        this->levelIO = io;
    }
    bool load(Level* level, const uint8_t* data, size_t length);
};