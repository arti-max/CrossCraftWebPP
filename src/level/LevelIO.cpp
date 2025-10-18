#include "LevelIO.hpp"
#include "CrossCraft.hpp"
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <zlib.h>
#include <cstring>
#include <iostream>

LevelIO::LevelIO(CrossCraft* cc) : cc(cc) {}

LevelIO::~LevelIO() {}

void LevelIO::writeInt32(std::vector<uint8_t>& buffer, int32_t value) {
    buffer.push_back((value >> 24) & 0xFF);
    buffer.push_back((value >> 16) & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back(value & 0xFF);
}

void LevelIO::writeInt16(std::vector<uint8_t>& buffer, int16_t value) {
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back(value & 0xFF);
}

void LevelIO::writeByte(std::vector<uint8_t>& buffer, uint8_t value) {
    buffer.push_back(value);
}

void LevelIO::writeInt64(std::vector<uint8_t>& buffer, int64_t value) {
    buffer.push_back((value >> 56) & 0xFF);
    buffer.push_back((value >> 48) & 0xFF);
    buffer.push_back((value >> 40) & 0xFF);
    buffer.push_back((value >> 32) & 0xFF);
    buffer.push_back((value >> 24) & 0xFF);
    buffer.push_back((value >> 16) & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back(value & 0xFF);
}

void LevelIO::writeUTF(std::vector<uint8_t>& buffer, const std::string& str) {
    writeInt16(buffer, static_cast<int16_t>(str.length()));
    for (char c : str) {
        buffer.push_back(static_cast<uint8_t>(c));
    }
}

int32_t LevelIO::readInt32(const uint8_t* data, size_t& offset) {
    int32_t value = (data[offset] << 24) | (data[offset+1] << 16) | 
                    (data[offset+2] << 8) | data[offset+3];
    offset += 4;
    return value;
}

int16_t LevelIO::readInt16(const uint8_t* data, size_t& offset) {
    int16_t value = (data[offset] << 8) | data[offset+1];
    offset += 2;
    return value;
}

int8_t LevelIO::readInt8(const uint8_t* data, size_t& offset) {
    int8_t value = data[offset];
    offset += 1;
    return value;
}

int64_t LevelIO::readInt64(const uint8_t* data, size_t& offset) {
    int64_t value = ((int64_t)data[offset] << 56) | ((int64_t)data[offset+1] << 48) |
                    ((int64_t)data[offset+2] << 40) | ((int64_t)data[offset+3] << 32) |
                    ((int64_t)data[offset+4] << 24) | ((int64_t)data[offset+5] << 16) |
                    ((int64_t)data[offset+6] << 8) | (int64_t)data[offset+7];
    offset += 8;
    return value;
}

std::string LevelIO::readUTF(const uint8_t* data, size_t& offset) {
    int16_t length = readInt16(data, offset);
    std::string str(reinterpret_cast<const char*>(data + offset), length);
    offset += length;
    return str;
}

std::vector<uint8_t> LevelIO::compressGzip(const std::vector<uint8_t>& data) {
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    
    if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        std::cerr << "Failed to initialize deflate" << std::endl;
        return {};
    }
    
    stream.avail_in = data.size();
    stream.next_in = const_cast<uint8_t*>(data.data());
    
    std::vector<uint8_t> compressed;
    compressed.resize(compressBound(data.size()));
    
    stream.avail_out = compressed.size();
    stream.next_out = compressed.data();
    
    deflate(&stream, Z_FINISH);
    deflateEnd(&stream);
    
    compressed.resize(stream.total_out);
    return compressed;
}

std::vector<uint8_t> LevelIO::decompressGzip(const uint8_t* data, size_t length) {
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = length;
    stream.next_in = const_cast<uint8_t*>(data);
    
    if (inflateInit2(&stream, 15 + 16) != Z_OK) {
        std::cerr << "Failed to initialize inflate" << std::endl;
        return {};
    }
    
    std::vector<uint8_t> decompressed;
    uint8_t buffer[32768];
    
    int ret;
    do {
        stream.avail_out = sizeof(buffer);
        stream.next_out = buffer;
        
        ret = inflate(&stream, Z_NO_FLUSH);
        
        if (ret != Z_OK && ret != Z_STREAM_END) {
            inflateEnd(&stream);
            std::cerr << "Inflate error: " << ret << std::endl;
            return {};
        }
        
        size_t have = sizeof(buffer) - stream.avail_out;
        decompressed.insert(decompressed.end(), buffer, buffer + have);
        
    } while (ret != Z_STREAM_END);
    
    inflateEnd(&stream);
    return decompressed;
}

struct LoadContext {
    LevelIO* levelIO;
    Level* level;
};

static void loadSuccessCallback(emscripten_fetch_t* fetch) {
    LoadContext* ctx = static_cast<LoadContext*>(fetch->userData);
    
    ctx->levelIO->cc->levelLoadUpdate("Loading...");
    
    if (fetch->numBytes < 2 || fetch->data[0] != 0x1f || (uint8_t)fetch->data[1] != 0x8b) {
        std::string errorMessage(fetch->data, fetch->numBytes);
        ctx->levelIO->cc->levelLoadUpdate(("Failed: " + errorMessage).c_str());
        emscripten_sleep(1000);
        emscripten_fetch_close(fetch);
        delete ctx;
        return;
    }
    
    bool result = ctx->levelIO->load(ctx->level, reinterpret_cast<const uint8_t*>(fetch->data), fetch->numBytes);
    
    if (!result) {
        ctx->levelIO->cc->levelLoadUpdate("Failed to parse level data");
        emscripten_sleep(1000);
    }
    
    emscripten_fetch_close(fetch);
    delete ctx;
}

static void loadErrorCallback(emscripten_fetch_t* fetch) {
    LoadContext* ctx = static_cast<LoadContext*>(fetch->userData);
    
    std::cerr << "HTTP error! Status: " << fetch->status << std::endl;
    ctx->levelIO->cc->levelLoadUpdate("Failed!");
    emscripten_sleep(1000);
    
    emscripten_fetch_close(fetch);
    delete ctx;
}

bool LevelIO::loadOnline(Level* level, const std::string& serverUrl, const std::string& user, int levelId) {
    cc->beginLevelLoading("Loading level");
    cc->levelLoadUpdate("Connecting...");
    
    std::string url = "http://" + serverUrl + "/level/load.html?id=" + std::to_string(levelId) + "&user=" + user;
    std::cout << "Loading level from: " << url << std::endl;
    
    LoadContext* ctx = new LoadContext{this, level};
    
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = loadSuccessCallback;
    attr.onerror = loadErrorCallback;
    attr.userData = ctx;
    
    emscripten_fetch(&attr, url.c_str());
    
    return true;
}

bool LevelIO::load(Level* level, const uint8_t* data, size_t length) {
    cc->levelLoadUpdate("Reading..");
    
    try {
        std::vector<uint8_t> decompressed = decompressGzip(data, length);
        
        if (decompressed.empty()) {
            std::cerr << "Failed to decompress data" << std::endl;
            return false;
        }
        
        size_t offset = 0;
        
        int32_t magicNumber = readInt32(decompressed.data(), offset);
        if (magicNumber != 656127880) {
            std::cerr << "Invalid magic number: " << magicNumber << std::endl;
            return false;
        }
        
        int8_t version = readInt8(decompressed.data(), offset);
        if (version > 1) {
            std::cerr << "Unsupported format version: " << (int)version << std::endl;
            return false;
        }
        
        std::string name = readUTF(decompressed.data(), offset);
        std::string creator = readUTF(decompressed.data(), offset);
        int64_t creationTime = readInt64(decompressed.data(), offset);
        
        int16_t width = readInt16(decompressed.data(), offset);
        int16_t height = readInt16(decompressed.data(), offset);
        int16_t depth = readInt16(decompressed.data(), offset);
        
        size_t blocksLength = width * height * depth;
        std::vector<uint8_t> blocks(decompressed.begin() + offset, 
                                    decompressed.begin() + offset + blocksLength);
        
        level->setData(width, depth, height, blocks);
        level->name = name;
        level->creator = creator;
        level->creationTime = creationTime;
        
        std::cout << "Level loaded: " << name << " (" << width << "x" << height << "x" << depth << ")" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse level data: " << e.what() << std::endl;
        return false;
    }
}

std::vector<uint8_t> LevelIO::serializeLevelToByteArray(Level* level) {
    std::vector<uint8_t> buffer;
    
    writeInt32(buffer, 656127880);
    writeByte(buffer, 1);
    writeUTF(buffer, level->name);
    writeUTF(buffer, level->creator);
    writeInt64(buffer, level->creationTime);
    
    writeInt16(buffer, level->width);
    writeInt16(buffer, level->height);
    writeInt16(buffer, level->depth);
    
    buffer.insert(buffer.end(), level->blocks.begin(), level->blocks.end());
    
    return compressGzip(buffer);
}

struct SaveContext {
    LevelIO* levelIO;
    std::string levelName;
    std::vector<uint8_t> body;
};

static void saveSuccessCallback(emscripten_fetch_t* fetch) {
    SaveContext* ctx = static_cast<SaveContext*>(fetch->userData);
    
    std::cout << "Save response status: " << fetch->status << std::endl;
    std::cout << "Save response size: " << fetch->numBytes << std::endl;
    
    if (fetch->status != 200) {
        std::string response(fetch->data, fetch->numBytes);
        std::cout << "Server error response: " << response << std::endl;
        
        ctx->levelIO->cc->levelLoadUpdate(("Failed! Status " + std::to_string(fetch->status)).c_str());
        emscripten_sleep(1000);
        emscripten_fetch_close(fetch);
        delete ctx;
        return;
    }
    
    std::string response(fetch->data, fetch->numBytes);
    std::cout << "Server response: '" << response << "'" << std::endl;
    
    // Trim пробелов
    response.erase(0, response.find_first_not_of(" \n\r\t"));
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
    
    if (response != "ok" && response != "OK") {
        ctx->levelIO->cc->levelLoadUpdate(("Failed: " + response).c_str());
        emscripten_sleep(1000);
    } else {
        ctx->levelIO->cc->levelLoadUpdate("Level successfully saved!");
        emscripten_sleep(1000);
    }
    
    emscripten_fetch_close(fetch);
    delete ctx;
}

static void saveErrorCallback(emscripten_fetch_t* fetch) {
    SaveContext* ctx = static_cast<SaveContext*>(fetch->userData);
    
    std::cerr << "Save HTTP error! Status: " << fetch->status << std::endl;
    ctx->levelIO->cc->levelLoadUpdate("Save failed!");
    emscripten_sleep(1000);
    
    emscripten_fetch_close(fetch);
    delete ctx;
}

bool LevelIO::saveOnline(Level* level, const std::string& serverUrl, const std::string& username, const std::string& sessionId, const std::string& levelName, int levelId) {
    
    cc->beginLevelLoading("Saving level");
    emscripten_sleep(1);
    
    cc->levelLoadUpdate("Compressing...");
    
    std::vector<uint8_t> compressedData = serializeLevelToByteArray(level);
    
    std::cout << "DEBUG: Compressed data size: " << compressedData.size() << std::endl;
    
    cc->levelLoadUpdate("Connecting...");
    emscripten_sleep(100);
    
    std::vector<uint8_t> body;
    
    // ОТЛАДКА: Выводим данные перед отправкой
    std::cout << "DEBUG: username length: " << username.length() << ", value: " << username << std::endl;
    std::cout << "DEBUG: sessionId length: " << sessionId.length() << ", value: " << sessionId << std::endl;
    std::cout << "DEBUG: levelName length: " << levelName.length() << ", value: " << levelName << std::endl;
    std::cout << "DEBUG: levelId: " << levelId << std::endl;
    
    writeUTF(body, username);
    writeUTF(body, sessionId.empty() ? "" : sessionId);
    writeUTF(body, levelName);
    writeByte(body, 0);  // progress
    writeInt32(body, compressedData.size());
    writeInt32(body, levelId);
    body.insert(body.end(), compressedData.begin(), compressedData.end());
    
    std::cout << "DEBUG: Total body size: " << body.size() << std::endl;
    std::cout << "DEBUG: First 20 bytes of body: ";
    for (int i = 0; i < std::min(20, (int)body.size()); i++) {
        printf("%02X ", body[i]);
    }
    std::cout << std::endl;
    
    SaveContext* ctx = new SaveContext{this, levelName, std::move(body)};
    
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = saveSuccessCallback;
    attr.onerror = saveErrorCallback;
    attr.userData = ctx;
    
    const char* headers[] = {"Content-Type", "application/octet-stream", nullptr};
    attr.requestHeaders = headers;
    
    attr.requestData = reinterpret_cast<const char*>(ctx->body.data());
    attr.requestDataSize = ctx->body.size();
    
    std::string url = "http://" + serverUrl + "/level/save.html";
    std::cout << "Saving level to: " << url << " (size: " << ctx->body.size() << " bytes)" << std::endl;
    
    emscripten_fetch(&attr, url.c_str());
    
    return true;
}
