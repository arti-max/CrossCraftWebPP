#include "sound/SoundType.hpp"


SoundType::SoundType(const std::string& name, float volume, float pitch, const std::string& soundName)
    : soundName(name), volume(volume), pitch(pitch), name(soundName) {}

SoundType::SoundType() : name(""), volume(1.0f), pitch(1.0f) {}

SoundType::~SoundType() {}

float SoundType::getVolume() const {
    return this->volume / (Random::random() * 0.4f + 1.0f) * 0.5f; 
}
float SoundType::getPitch() const {
    return this->pitch / (Random::random() * 0.2f + 0.9f); 
}

const SoundType SoundType::none     = SoundType("none", 0.0f, 0.0f, "-");

const SoundType SoundType::grass    = SoundType("grass", 1.0f, 1.0f, "grass");
const SoundType SoundType::stone    = SoundType("stone", 1.0f, 1.0f, "stone");
const SoundType SoundType::cloth    = SoundType("cloth", 1.0f, 1.2f, "grass");
const SoundType SoundType::gravel   = SoundType("gravel", 1.0f, 1.0f, "gravel");
const SoundType SoundType::metal    = SoundType("metal", 1.0f, 2.0f, "stone");
const SoundType SoundType::wood    = SoundType("wood", 1.0f, 1.0f, "wood");
const SoundType SoundType::leaves    = SoundType("leaves", 1.0f, 1.0f, "leaves");