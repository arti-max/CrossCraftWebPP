#include "LevelIO.hpp"
#include "CrossCraft.hpp"
#include "Entity.hpp"
#include "player/Player.hpp"
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <zlib.h>
#include <cstring>
#include <iostream>
// mobs
#include "mob/Zombie.hpp"
#include "mob/Creeper.hpp"
#include "mob/Skeleton.hpp"
#include "mob/AnimalMob.hpp"
#include "mob/HumanMob.hpp"
// items
#include "item/Item.hpp"
#include "item/Arrow.hpp"
#include "item/Sign.hpp"

const int ITEM_ENTITY = 32356;

static int SPAWNED_MOBS = 0;

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

void LevelIO::writeBool(std::vector<uint8_t>& buffer, bool value) {
    if (value == true) {
        buffer.push_back(1);
    } else {
        buffer.push_back(0);
    }
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

void LevelIO::writeFloat(std::vector<uint8_t>& buffer, float value) {
    int32_t intValue;
    memcpy(&intValue, &value, sizeof(float));
    writeInt32(buffer, intValue);
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

bool LevelIO::readBool(const uint8_t* data, size_t& offset) {
    int8_t value = data[offset];
    offset += 1;
    if (value == 1) {
        return true;
    } else {
        return false;
    }
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

float LevelIO::readFloat(const uint8_t* data, size_t& offset) {
    int32_t intValue = readInt32(data, offset);
    float value;
    memcpy(&value, &intValue, sizeof(float));
    return value;
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
    } else {
        ctx->levelIO->cc->player->resetPos();
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
    
    std::string url = "https://" + serverUrl + "/level/load.html?id=" + std::to_string(levelId) + "&user=" + user;
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
    cc->levelLoadProgress(10);
    
    try {
        std::vector<uint8_t> decompressed = decompressGzip(data, length);
        cc->levelLoadUpdate("Decompressing..");
        cc->levelLoadProgress(30);
        
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
        if (version > 3) {
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
        std::vector<uint8_t> blocks(decompressed.begin() + offset, decompressed.begin() + offset + blocksLength);
        offset += blocksLength;
        
        Player* player = (Player*)level->player;
        if (player == nullptr) {
            std::cerr << "LEVELIO: PLAYEr IS EMPTY!!!!" << std::endl;
            return false;
        }
        
        level->setData(width, depth, height, blocks);
        level->name = name;
        level->creator = creator;
        level->creationTime = creationTime;

        level->addEntity(player);

        if (version == 2) {
            level->xSpawn = readInt16(decompressed.data(), offset);
            level->ySpawn = readInt16(decompressed.data(), offset);
            level->zSpawn = readInt16(decompressed.data(), offset);
            level->rotSpawn = readInt16(decompressed.data(), offset);

            int32_t entityCount = readInt32(decompressed.data(), offset);

            for (int i = 0; i < entityCount; ++i) {
                int32_t entityTypeId = readInt32(decompressed.data(), offset);
                
                if (entityTypeId == 1) { // 1 = Zombie
                    float loadedX = readFloat(decompressed.data(), offset);
                    float loadedY = readFloat(decompressed.data(), offset);
                    float loadedZ = readFloat(decompressed.data(), offset);
                    float loadedXRot = readFloat(decompressed.data(), offset);
                    float loadedYRot = readFloat(decompressed.data(), offset);
                    
                    HumanMob* zombie = new HumanMob(level, loadedX, loadedY, loadedZ);
                    zombie->xRot = loadedXRot;
                    zombie->yRot = loadedYRot;
                    zombie->setPos(loadedX, loadedY, loadedZ);
                    level->addEntity(zombie);
                } else {
                    offset += sizeof(float) * 5; 
                }
            }

            for (int i = 0; i < 9; i++) {
                player->inventory->slots[i] = -1;
                player->inventory->count[i] = 0;
            }
        }

        if (version >= 3) {
            SPAWNED_MOBS = 0;
            // read player position
            level->xSpawn = readInt16(decompressed.data(), offset);
            level->ySpawn = readInt16(decompressed.data(), offset);
            level->zSpawn = readInt16(decompressed.data(), offset);
            level->rotSpawn = readInt16(decompressed.data(), offset);

            // read player stats
            player->health = readInt16(decompressed.data(), offset);
            player->airSupply = readInt16(decompressed.data(), offset);
            player->score = readInt16(decompressed.data(), offset);

            // read inventory data
            player->inventory->arrows = readInt16(decompressed.data(), offset);
            for (int i = 0; i < 9; i++) {
                player->inventory->slots[i] = readInt16(decompressed.data(), offset);
                player->inventory->count[i] = readInt16(decompressed.data(), offset);
            }

            // read entity data
            int32_t entityCount = readInt32(decompressed.data(), offset);
            Logger::logf(PREFIX_DEBUG, "Entity Count: %i, offset: %i\n", entityCount, offset);
            for (int i = 0; i < entityCount; ++i) {
                int32_t entityTypeId = readInt32(decompressed.data(), offset);
                Entity* mob = nullptr;
                if (entityTypeId == ITEM_ENTITY) {
                    int8_t itemId = readInt8(decompressed.data(), offset);
                    if (itemId == 1) { // Block Drop
                        int8_t resourceId = readInt8(decompressed.data(), offset);
                        float loadedX = readFloat(decompressed.data(), offset);
                        float loadedY = readFloat(decompressed.data(), offset);
                        float loadedZ = readFloat(decompressed.data(), offset);

                        mob = new Item(level, loadedX, loadedY, loadedZ, resourceId);
                    }
                    if (itemId == 2) { // Arrow
                        int8_t arrowType = readInt8(decompressed.data(), offset);
                        if (arrowType == 0) {
                            bool hit = readBool(decompressed.data(), offset);
                            float gravity = readFloat(decompressed.data(), offset);
                            float loadedX = readFloat(decompressed.data(), offset);
                            float loadedY = readFloat(decompressed.data(), offset);
                            float loadedZ = readFloat(decompressed.data(), offset);
                            float xRot = readFloat(decompressed.data(), offset);
                            float yRot = readFloat(decompressed.data(), offset);

                            mob = new Arrow(level, player, loadedX, loadedY, loadedZ, xRot, yRot, gravity);
                            Arrow* newArrow = (Arrow*)mob;
                            newArrow->hasHit = hit;
                            newArrow->type = arrowType;
                        }
                    }
                    if (mob != nullptr) {
                        SPAWNED_MOBS++;
                        level->addEntity(mob);
                    } else {
                        std::cerr << "Item is nullptr!" << std::endl;
                    }
                } else {
                    float loadedX = readFloat(decompressed.data(), offset);
                    float loadedY = readFloat(decompressed.data(), offset);
                    float loadedZ = readFloat(decompressed.data(), offset);
                    
                    switch(entityTypeId) {
                        case 1: mob = new Zombie(level, loadedX, loadedY, loadedZ); break;
                        case 2: mob = new Skeleton(level, loadedX, loadedY, loadedZ); break;
                        case 3: mob = new AnimalMob(level, loadedX, loadedY, loadedZ); break;
                        case 4: mob = new Creeper(level, loadedX, loadedY, loadedZ); break;
                        // case 5: mob = new HumanMob(level, loadedX, loadedY, loadedZ); break;
                    }

                    if (mob != nullptr) {
                        level->addEntity(mob);
                    } else {
                        std::cerr << "Mob is nullptr! Type: " << entityTypeId << ", offset: " << offset << std::endl;
                    }
                }
            }
        }

        cc->levelLoadUpdate("Finalizing.."); 
        cc->levelLoadProgress(90);
        
        std::cout << "Level loaded: " << name << " (" << width << "x" << height << "x" << depth << ") spawned: " << SPAWNED_MOBS << std::endl;
        
        cc->levelLoadProgress(100);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse level data: " << e.what() << std::endl;
        return false;
    }
}

std::vector<uint8_t> LevelIO::serializeLevelToByteArray(Level* level) {
    std::vector<uint8_t> buffer;

    Player* player = (Player*)level->player;
    
    writeInt32(buffer, 656127880);
    writeByte(buffer, 3); // level format
    writeUTF(buffer, level->name);
    writeUTF(buffer, level->creator);
    writeInt64(buffer, level->creationTime);
    
    writeInt16(buffer, level->width);
    writeInt16(buffer, level->height);
    writeInt16(buffer, level->depth);
    
    buffer.insert(buffer.end(), level->blocks.begin(), level->blocks.end());
    
    // player position
    writeInt16(buffer, (int)level->player->x);
    writeInt16(buffer, (int)level->player->y);
    writeInt16(buffer, (int)level->player->z);
    writeInt16(buffer, (int)level->player->xRot);

    // player stats
    writeInt16(buffer, player->health);
    writeInt16(buffer, player->airSupply);
    writeInt16(buffer, player->score);

    // save inventory
    writeInt16(buffer, player->inventory->arrows);
    for (int i = 0; i < 9; i++) {
        writeInt16(buffer, player->inventory->slots[i]);
        writeInt16(buffer, player->inventory->count[i]);
    }
    
    if (level->emesh->all.size() <= 0) {
        std::cerr << "Entity list is null!" << std::endl;
    }

    // entities
    writeInt32(buffer, level->emesh->all.size());
    for (Entity* entity : level->emesh->all) {
        if (dynamic_cast<Zombie*>(entity)) {
            writeInt32(buffer, 1); // 1 = Zombie
            writeFloat(buffer, entity->x);
            writeFloat(buffer, entity->y);
            writeFloat(buffer, entity->z);
        }
        if (dynamic_cast<Skeleton*>(entity)) {
            writeInt32(buffer, 2); // 2 = Skeleton
            writeFloat(buffer, entity->x);
            writeFloat(buffer, entity->y);
            writeFloat(buffer, entity->z);
        }
        if (dynamic_cast<AnimalMob*>(entity)) {
            writeInt32(buffer, 3); // 3 = Animal
            writeFloat(buffer, entity->x);
            writeFloat(buffer, entity->y);
            writeFloat(buffer, entity->z);
        }
        if (dynamic_cast<Creeper*>(entity)) {
            writeInt32(buffer, 4); // 4 = Creeper
            writeFloat(buffer, entity->x);
            writeFloat(buffer, entity->y);
            writeFloat(buffer, entity->z);
        }
        if (dynamic_cast<HumanMob*>(entity)) {
            writeInt32(buffer, 5); // 5 = Human Mob
            writeFloat(buffer, entity->x);
            writeFloat(buffer, entity->y);
            writeFloat(buffer, entity->z);
        }
        // items
        if (dynamic_cast<Item*>(entity)) {
            Item* item = (Item*)entity;
            writeInt32(buffer, ITEM_ENTITY);
            writeByte(buffer, 1); // 1 = Block Drop
            writeByte(buffer, item->getResourceId());
            writeFloat(buffer, entity->x);
            writeFloat(buffer, entity->y);
            writeFloat(buffer, entity->z);
        }
        if (dynamic_cast<Arrow*>(entity)) {
            Arrow* arrow = (Arrow*)entity;
            if (arrow->type == 0) {
                writeInt32(buffer, ITEM_ENTITY);
                writeByte(buffer, 2); // 2 = Arrow
                writeByte(buffer, arrow->type);
                writeBool(buffer, arrow->hasHit);
                writeFloat(buffer, arrow->gravity);
                writeFloat(buffer, entity->x);
                writeFloat(buffer, entity->y);
                writeFloat(buffer, entity->z);
                writeFloat(buffer, entity->xRot);
                writeFloat(buffer, entity->yRot);
            }
        }
    }

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
        ctx->levelIO->cc->levelLoadProgress(100);
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
    cc->levelLoadProgress(30);
    std::vector<uint8_t> compressedData = serializeLevelToByteArray(level);
    
    std::cout << "DEBUG: Compressed data size: " << compressedData.size() << std::endl;
    
    cc->levelLoadUpdate("Connecting...");
    cc->levelLoadProgress(60);
    emscripten_sleep(100);
    
    std::vector<uint8_t> body;
    
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
    
    std::string url = "https://" + serverUrl + "/level/save.html";
    std::cout << "Saving level to: " << url << " (size: " << ctx->body.size() << " bytes)" << std::endl;
    
    emscripten_fetch(&attr, url.c_str());
    
    return true;
}
