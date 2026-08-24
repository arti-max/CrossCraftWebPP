#include "level/io/v4.hpp"
#include "CrossCraft.hpp"
#include "level/LevelIO.hpp"

bool v4io::load(Level* level, const uint8_t* data, size_t length) {
        try {
        std::vector<uint8_t> decompressed = this->levelIO->decompressGzip(data, length);
        this->levelIO->cc->levelLoadUpdate("Decompressing..");
        this->levelIO->cc->levelLoadProgress(30);
        
        if (decompressed.empty()) {
            std::cerr << "Failed to decompress data" << std::endl;
            return false;
        }
        
        size_t offset = 0;
        
        int32_t magicNumber = this->levelIO->readInt32(decompressed.data(), offset);
        if (magicNumber != MAGIC) {
            std::cerr << "Invalid magic number: " << magicNumber << std::endl;
            return false;
        }
        
        int8_t version = this->levelIO->readInt8(decompressed.data(), offset);
        if (version > IO_VERSION) {
            std::cerr << "Unsupported format version: " << (int)version << std::endl;
            return false;
        }

        Logger::logf(PREFIX_DEBUG, "Before name!\n");
        
        std::string name = this->levelIO->readUTF(decompressed.data(), offset);
        std::string creator = this->levelIO->readUTF(decompressed.data(), offset);
        int64_t creationTime = this->levelIO->readInt64(decompressed.data(), offset);

        Logger::logf(PREFIX_DEBUG, "Before level size!\n");
        
        int16_t width = this->levelIO->readInt16(decompressed.data(), offset);
        int16_t height = this->levelIO->readInt16(decompressed.data(), offset);
        int16_t depth = this->levelIO->readInt16(decompressed.data(), offset);
        
        size_t blocksLength = width * height * depth;
        std::vector<uint8_t> blocks(decompressed.begin() + offset, decompressed.begin() + offset + blocksLength);
        offset += blocksLength;

        Logger::logf(PREFIX_DEBUG, "Before Player ptr update!\n");
        
        Player* player = (Player*)level->player;
        bool newCreated = false;
        if (player == nullptr) {
            std::cerr << "LEVELIO: PLAYEr IS EMPTY!!!!" << std::endl;
            return false;
        }
        if (player->dead == true){
            player = new Player(CrossCraft::instance->level, CrossCraft::instance->settings);
            newCreated = true;
            Logger::logf(PREFIX_DEBUG, "Player is dead, create new player object...\n");
        }

        Logger::logf(PREFIX_DEBUG, "Before update level data!\n");

        player->removeExternally = true;
        
        level->setData(width, depth, height, blocks);
        level->name = name;
        level->creator = creator;
        level->creationTime = creationTime;

        Logger::logf(PREFIX_DEBUG, "Before add player to Level!\n");
        Logger::logf(PREFIX_DEBUG, "Entity Data: %i, health: %i, dead: %i\n", player->removeExternally, player->health, player->dead);
        
        level->addEntity(player);
        Logger::logf(PREFIX_DEBUG, "Before add player to ptr in level object!\n");
        level->player = player;

        Logger::logf(PREFIX_DEBUG, "Before load spawn data!\n");
        
        // read player position
        level->xSpawn = this->levelIO->readInt16(decompressed.data(), offset);
        level->ySpawn = this->levelIO->readInt16(decompressed.data(), offset);
        level->zSpawn = this->levelIO->readInt16(decompressed.data(), offset);
        level->rotSpawn = this->levelIO->readInt16(decompressed.data(), offset);

        Logger::logf(PREFIX_DEBUG, "Before load player states!\n");

        // read player stats
        player->health = this->levelIO->readInt16(decompressed.data(), offset);
        player->airSupply = this->levelIO->readInt16(decompressed.data(), offset);
        player->score = this->levelIO->readInt16(decompressed.data(), offset);

        Logger::logf(PREFIX_DEBUG, "Before load player inventory!\n");

        // read inventory data
        player->inventory->arrows = this->levelIO->readInt16(decompressed.data(), offset);
        for (int i = 0; i < 9; i++) {
            player->inventory->slots[i] = this->levelIO->readInt16(decompressed.data(), offset);
            player->inventory->count[i] = this->levelIO->readInt16(decompressed.data(), offset);
        }

        Logger::logf(PREFIX_DEBUG, "Before load entity data in v3!\n");

        // read entity data
        int32_t entityCount = this->levelIO->readInt32(decompressed.data(), offset);
        for (int i = 0; i < entityCount; ++i) {
            int32_t entityTypeId = this->levelIO->readInt32(decompressed.data(), offset);
            Entity* mob = nullptr;
            if (entityTypeId == ITEM_ENTITY) {
                int8_t itemId = this->levelIO->readInt8(decompressed.data(), offset);
                if (itemId == 1) { // Block Drop
                    int8_t resourceId = this->levelIO->readInt8(decompressed.data(), offset);
                    float loadedX = this->levelIO->readFloat(decompressed.data(), offset);
                    float loadedY = this->levelIO->readFloat(decompressed.data(), offset);
                    float loadedZ = this->levelIO->readFloat(decompressed.data(), offset);

                    mob = new Item(level, loadedX, loadedY, loadedZ, resourceId);
                }
                if (itemId == 2) { // Arrow
                    int8_t arrowType = this->levelIO->readInt8(decompressed.data(), offset);
                    if (arrowType == 0) {
                        bool hit = this->levelIO->readBool(decompressed.data(), offset);
                        float gravity = this->levelIO->readFloat(decompressed.data(), offset);
                        float loadedX = this->levelIO->readFloat(decompressed.data(), offset);
                        float loadedY = this->levelIO->readFloat(decompressed.data(), offset);
                        float loadedZ = this->levelIO->readFloat(decompressed.data(), offset);
                        float xRot = this->levelIO->readFloat(decompressed.data(), offset);
                        float yRot = this->levelIO->readFloat(decompressed.data(), offset);

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
                float loadedX = this->levelIO->readFloat(decompressed.data(), offset);
                float loadedY = this->levelIO->readFloat(decompressed.data(), offset);
                float loadedZ = this->levelIO->readFloat(decompressed.data(), offset);
                
                switch(entityTypeId) {
                    case 1: mob = new Zombie(level, loadedX, loadedY, loadedZ); break;
                    case 2: mob = new Skeleton(level, loadedX, loadedY, loadedZ); break;
                    case 3: mob = new AnimalMob(level, loadedX, loadedY, loadedZ); break;
                    case 4: mob = new Creeper(level, loadedX, loadedY, loadedZ); break;
                    case 5: mob = new Spider(level, loadedX, loadedY, loadedZ); break;
                }

                if (mob != nullptr) {
                    level->addEntity(mob);
                } else {
                    std::cerr << "Mob is nullptr! Type: " << entityTypeId << ", offset: " << offset << std::endl;
                }
            }
        }

        Logger::logf(PREFIX_DEBUG, "Before load end!\n");

        this->levelIO->cc->levelLoadUpdate("Finalizing.."); 
        this->levelIO->cc->levelLoadProgress(90);
        
        std::cout << "Level loaded: " << name << " (" << width << "x" << height << "x" << depth << ") spawned: " << std::endl;
        
        this->levelIO->cc->levelLoadProgress(100);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse level data: " << e.what() << std::endl;
        return false;
    }
}