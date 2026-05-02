#pragma once

#include <string>
#include "util/Random.hpp"


class SoundType {
private:
public:
    float volume = 0.0f;
    float pitch = 0.0f;
    std::string name = "";
    std::string soundName = "";

    SoundType(const std::string& name, float volume, float pitch, const std::string& soundName);
    SoundType();
    ~SoundType();
    float getVolume() const;
    float getPitch() const;


    static const SoundType none;
    static const SoundType grass;
    static const SoundType stone;
    static const SoundType gravel;
    static const SoundType metal;
    static const SoundType cloth;
    static const SoundType wood;
    static const SoundType leaves;
};