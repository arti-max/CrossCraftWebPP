#pragma once
#include "level/io/v4.hpp"

class LevelIO;

class LoadLevelAPI {
private:
    v4io* V4 = new v4io();

public:
    LoadLevelAPI(LevelIO* levelIO) {
        this->V4->setIO(levelIO);
    }

    // мульти-класс для загрузки данных мира по разный версиям апи, которые лежат в разных файлах v<версия апи>.hpp/.cpp
    bool loadByVersion(int version, Level* level, const uint8_t* data, size_t length) {
        switch (version) {
            case 4: return this->V4->load(level, data, length); break;
        }
        return false;
    }
};