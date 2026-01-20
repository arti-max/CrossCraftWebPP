#pragma once

#include <string>
#include <vector>
#include <map>
#include <AL/al.h>
#include <AL/alc.h>
#include "util/Logger.hpp"
#include "util/Random.hpp"
#include <emscripten.h>
#include "Settings.hpp"

struct SoundData {
    ALuint bufferId;
};

struct SoundGroup {
    std::vector<SoundData> variants;
};


struct MusicGroup {
    std::vector<SoundData> variants;
};


class SoundManager {
private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
    Random random;

    bool muted = false;
    Settings* settings;

    std::map<std::string, SoundGroup> soundLibrary;
    std::map<std::string, MusicGroup> musicLibrary;
    std::vector<ALuint> sources;

    ALuint musicSource = 0;

    ALuint getFreeSource();

public:
    SoundManager();
    ~SoundManager();
    void initOpenAL(Settings* settings);
    

    bool mutedSound = false;
    double lastMusic = emscripten_get_now() + 20000;

    bool loadSound(const std::string& name, const std::string& filepath);
    void play(const std::string& name, float ptich = 1.0f, float gain = 1.0f);

    void downloadAndRegister(const std::string& path, const std::string& name);
    void downloadAndRegisterMusic(const std::string& path, const std::string& name); 

    void onSoundDownloaded(std::string name, void* data, int size);
    void onMusicDownloaded(std::string name, void* data, int size);

    bool playMusic(const std::string& name);
    void playAt(const std::string& name, float x, float y, float z, float pitch = 1.0f, float gain = 1.0f);
    void updateListener(float x, float y, float z, float yaw, float pitch);

    void setMuted(bool mute);
    bool isMuted() const { return muted; }
    void toggleMute();

    void tick();
    void cleanup();
};