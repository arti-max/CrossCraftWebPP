#include "sound/SoundManager.hpp"
#include "sound/ResourceDownloader.hpp"
#include <emscripten/html5.h>
#include "stb_vorbis.h"
#include "Random.hpp"

#define AL_SOURCE_SPATIALIZE_SOFT  0x1214
#define AL_AUTO_SOFT               0x0002

static const int MAX_SOURCES = 128;

SoundManager::SoundManager() {}
SoundManager::~SoundManager() {}


void SoundManager::initOpenAL(Settings* settings) {
    this->settings = settings;
    this->device = alcOpenDevice(nullptr);
    this->context = alcCreateContext(this->device, nullptr);
    alcMakeContextCurrent(this->context);
    alcIsExtensionPresent(device, "ALC_SOFT_HRTF");
    this->sources.resize(MAX_SOURCES);
    alGenSources(MAX_SOURCES, this->sources.data());

    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    alGenSources(1, &this->musicSource);
    alSourcei(this->musicSource, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(this->musicSource, AL_ROLLOFF_FACTOR, 0.0f);
    alSource3f(this->musicSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
    // Sounds
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/grass1.ogg", "step.grass");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/grass2.ogg", "step.grass");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/grass3.ogg", "step.grass");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/grass4.ogg", "step.grass");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/stone1.ogg", "step.stone");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/stone2.ogg", "step.stone");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/stone3.ogg", "step.stone");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/stone4.ogg", "step.stone");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/wood1.ogg", "step.wood");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/wood2.ogg", "step.wood");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/wood3.ogg", "step.wood");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/wood4.ogg", "step.wood");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/gravel1.ogg", "step.gravel");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/gravel2.ogg", "step.gravel");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/gravel3.ogg", "step.gravel");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/gravel4.ogg", "step.gravel");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/leaves1.ogg", "step.leaves");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/leaves2.ogg", "step.leaves");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/leaves3.ogg", "step.leaves");
    this->downloadAndRegister("https://crosscraftweb.ddns.net/resources/steps/leaves4.ogg", "step.leaves");

    // Music
    this->downloadAndRegisterMusic("https://crosscraftweb.ddns.net/resources/music/calm1.ogg", "calm");
    this->downloadAndRegisterMusic("https://crosscraftweb.ddns.net/resources/music/calm2.ogg", "calm");
    this->downloadAndRegisterMusic("https://crosscraftweb.ddns.net/resources/music/calm3.ogg", "calm");
}

void SoundManager::downloadAndRegister(const std::string& path, const std::string& name) {
    auto cb = [this](std::string n, void* d, int s) {
        this->onSoundDownloaded(n, d, s);
    };

    ResourceDownloader::loadSound(path, name, cb);
}

void SoundManager::downloadAndRegisterMusic(const std::string& path, const std::string& name) {
    auto cb = [this](std::string n, void* d, int s) {
        this->onMusicDownloaded(n, d, s);
    };
    ResourceDownloader::loadSound(path, name, cb);
}

void SoundManager::onMusicDownloaded(std::string name, void* data, int size) {
    if (!data || size <= 0) return;

    int channels, sampleRate;
    short* pcmData;
    int samples = stb_vorbis_decode_memory((const unsigned char*)data, size, &channels, &sampleRate, &pcmData);

    if (samples < 0) {
        Logger::logf(PREFIX_ERROR, "Failed to decode music: %s\n", name.c_str());
        free(data);
        return;
    }

    ALenum format;
    if (channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        Logger::logf(PREFIX_ERROR, "(music) Unsupported channel count: %d in %s\n", channels, name.c_str());
        free(pcmData);
        free(data);
        return;
    }

    ALuint bufferId;
    alGenBuffers(1, &bufferId);
    alBufferData(bufferId, format, pcmData, samples * channels * sizeof(short), sampleRate);

    SoundData sd;
    sd.bufferId = bufferId;
    this->musicLibrary[name].variants.push_back(sd);

    Logger::logf(PREFIX_DEBUG, "Loaded music: %s (id: %d)\n", name.c_str(), bufferId);

    free(pcmData);
    free(data);
}

void SoundManager::onSoundDownloaded(std::string name, void* data, int size) {
    if (!data || size <= 0) {
        Logger::logf(PREFIX_ERROR, "Sound download failed or empty: %s\n", name.c_str());
        return;
    }

    int channels, sampleRate;
    short* pcmData;

    int samples = stb_vorbis_decode_memory((const unsigned char*)data, size, &channels, &sampleRate, &pcmData);

    if (samples < 0) {
        Logger::logf(PREFIX_ERROR, "Failed to decode OGG: %s\n", name.c_str());
        free(data);
        return;
    }

    ALenum format;
    if (channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        Logger::logf(PREFIX_ERROR, "(sound) Unsupported channel count: %d in %s\n", channels, name.c_str());
        free(pcmData);
        free(data);
        return;
    }

    ALuint bufferId;
    alGenBuffers(1, &bufferId);

    alBufferData(bufferId, format, pcmData, samples * channels * sizeof(short), sampleRate);

    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        Logger::logf(PREFIX_ERROR, "OpenAL Buffer Error: %d for %s\n", err, name.c_str());
    } else {
        SoundData sd;
        sd.bufferId = bufferId;

        this->soundLibrary[name].variants.push_back(sd);

        Logger::logf(PREFIX_DEBUG, "Loaded sound: %s (id: %d)\n", name.c_str(), bufferId);
    }

    free(pcmData);
    free(data);
}

void SoundManager::play(const std::string& name, float pitch, float gain) {
    this->playAt(name, 0, 0, 0, pitch, gain);
}

bool SoundManager::playMusic(const std::string& name) {
    if (this->muted || this->settings->music == false) return false;

    ALint state;
    alGetSourcei(this->musicSource, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {
        return false;
    }

    auto it = this->musicLibrary.find(name);
    if (it == this->musicLibrary.end() || it->second.variants.empty()) {
        return false;
    }

    const std::vector<SoundData>& variants = it->second.variants;
    int index = rand() % variants.size();
    ALuint bufferId = variants[index].bufferId;

    alSourcei(this->musicSource, AL_BUFFER, 0);
    alSourcei(this->musicSource, AL_BUFFER, bufferId);
    alSourcef(this->musicSource, AL_GAIN, 0.5f);
    alSourcef(this->musicSource, AL_PITCH, 1.0f);
    alSourcePlay(this->musicSource);

    Logger::logf(PREFIX_DEBUG, "Started playing background music: %s (track %d)\n", name.c_str(), index);
    return true;
}

void SoundManager::playAt(const std::string& name, float x, float y, float z, float pitch, float gain) {
    if (this->muted || this->settings->sound == false) return;

    auto it = this->soundLibrary.find(name);
    if (it == this->soundLibrary.end() || it->second.variants.empty()) {
        return;
    }

    const std::vector<SoundData>& variants = it->second.variants;
    int index = 0;
    if (variants.size() > 1) {
        index = rand() % variants.size();
    }
    ALuint bufferId = variants[index].bufferId;
    ALuint source = this->getFreeSource();
    if (source == 0) return;

    alSourceStop(source);
    alSourcei(source, AL_BUFFER, 0);
    alGetError();

    alSourcei(source, AL_BUFFER, bufferId);
    alSourcef(source, AL_PITCH, pitch);
    alSourcef(source, AL_GAIN, gain);
    alSource3f(source, AL_POSITION, x, y, z);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);

    alSourcef(source, AL_REFERENCE_DISTANCE, 5.0f);
    alSourcef(source, AL_MAX_DISTANCE, 20.0f);
    alSourcef(source, AL_ROLLOFF_FACTOR, 0.5f);

    if (alcIsExtensionPresent(device, "AL_SOFT_source_spatialize")) {
        alSourcei(source, AL_SOURCE_SPATIALIZE_SOFT, AL_FALSE);
    }

    alSourcePlay(source);

    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        Logger::logf(PREFIX_ERROR, "OpenAL play error: %d for sound '%s'\n", err, name.c_str());
    }
}

void SoundManager::playCentered(const std::string& name, float pitch, float gain) {
    if (this->muted || this->settings->sound == false) return;

    auto it = this->soundLibrary.find(name);
    if (it == this->soundLibrary.end() || it->second.variants.empty()) {
        return;
    }

    const std::vector<SoundData>& variants = it->second.variants;
    int index = 0;
    if (variants.size() > 1) {
        index = rand() % variants.size();
    }
    ALuint bufferId = variants[index].bufferId;
    ALuint source = this->getFreeSource();
    if (source == 0) return;

    alSourceStop(source);
    alSourcei(source, AL_BUFFER, 0);
    alGetError();

    alSourcei(source, AL_BUFFER, bufferId);
    alSourcef(source, AL_PITCH, pitch);
    alSourcef(source, AL_GAIN, gain);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

    alSourcePlay(source);

    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        Logger::logf(PREFIX_ERROR, "OpenAL play error: %d\n", err);
    }
}

void SoundManager::updateListener(float x, float y, float z, float yaw, float pitch) {
    alListener3f(AL_POSITION, x, y, z);
    alListener3f(AL_VELOCITY, 0, 0, 0);

    float radYaw = -yaw * (M_PI / 180.0f);
    float radPitch = -pitch * (M_PI / 180.0f);

    float lookX = -sin(radYaw) * cos(radPitch);
    float lookY = sin(radPitch);
    float lookZ = -cos(radYaw) * cos(radPitch);

    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;

    float orientation[6] = {lookX, lookY, lookZ, upX, upY, upZ};
    alListenerfv(AL_ORIENTATION, orientation);
}

ALuint SoundManager::getFreeSource() {
    for (ALuint src : this->sources) {
        ALint state;
        alGetSourcei(src, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            return src;
        }
    }

    for (ALuint src : this->sources) {
        ALint state;
        alGetSourcei(src, AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING) {
            alSourceStop(src);
            return src;
        }
    }

    return 0;
}

void SoundManager::toggleMute() {
    this->setMuted(!this->muted);
}

void SoundManager::tick() {
    Random* random = new Random();

    if (emscripten_get_now() > this->lastMusic && this->playMusic("calm") && this->settings->music) {
        this->lastMusic = emscripten_get_now() + random->nextInt(900000) + 300000;
    }

    if (this->settings->music == false) {
        alSourceStop(this->musicSource);
    }

    if (this->settings->sound == false) {
        for (ALuint src : this->sources) {
            ALint state;
            alGetSourcei(src, AL_SOURCE_STATE, &state);
            if (state == AL_PLAYING) {
                alSourceStop(src);
            }
        }
    }

    delete random;
}

void SoundManager::setMuted(bool mute) {
    this->muted = mute;

    if (this->muted) {
        alSourceStop(this->musicSource);

        for (ALuint src : this->sources) {
            ALint state;
            alGetSourcei(src, AL_SOURCE_STATE, &state);
            if (state == AL_PLAYING) {
                alSourceStop(src);
            }
        }
        Logger::logf(PREFIX_CC, "Sound muted\n");
    } else {
        this->lastMusic = emscripten_get_now() + 16000;
        Logger::logf(PREFIX_CC, "Sound unmuted\n");
    }
}