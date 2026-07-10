#pragma once
#include "Level.hpp"
#include "LevelLoaderListener.hpp"
#include <string>
#include <vector>
#include <cstdint>

class CrossCraft;

class LevelIO {
private:
    std::vector<uint8_t> serializeLevelToByteArray(Level* level);
    
    void writeInt32(std::vector<uint8_t>& buffer, int32_t value);
    void writeInt16(std::vector<uint8_t>& buffer, int16_t value);
    void writeByte(std::vector<uint8_t>& buffer, uint8_t value);
    void writeBool(std::vector<uint8_t>& buffer, bool value);
    void writeInt64(std::vector<uint8_t>& buffer, int64_t value);
    void writeUTF(std::vector<uint8_t>& buffer, const std::string& str);
    void writeFloat(std::vector<uint8_t>& buffer, float value);
    
    int32_t readInt32(const uint8_t* data, size_t& offset);
    int16_t readInt16(const uint8_t* data, size_t& offset);
    int8_t readInt8(const uint8_t* data, size_t& offset);
    bool readBool(const uint8_t* data, size_t& offset);
    int64_t readInt64(const uint8_t* data, size_t& offset);
    std::string readUTF(const uint8_t* data, size_t& offset);
    float readFloat(const uint8_t* data, size_t& offset);

public:
    CrossCraft* cc;
    
    LevelIO(CrossCraft* cc);
    ~LevelIO();

    std::vector<uint8_t> compressGzip(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressGzip(const uint8_t* data, size_t length);
    
    bool loadOnline(Level* level, const std::string& serverUrl, const std::string& user, int levelId);
    bool load(Level* level, const uint8_t* data, size_t length);
    bool saveOnline(Level* level, const std::string& serverUrl, const std::string& username, 
                   const std::string& sessionId, const std::string& levelName, int levelId);
};
