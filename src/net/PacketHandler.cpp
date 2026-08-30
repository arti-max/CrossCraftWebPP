#include "net/PacketHandler.hpp"
#include "net/packet/Packet.hpp"
#include "util/Logger.hpp"
#include "CrossCraft.hpp"

void PacketHandler::handleNetworkPacket(Packet* packet) {
    switch (packet->getType()) {
        case PacketType::SERVER_IDENTIFICATION: {
            ServerIdentificationPacket* p = static_cast<ServerIdentificationPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Server Identification received: %s\n", p->serverName.c_str());
            
            this->cc->beginLevelLoading(p->serverName.c_str());
            this->cc->levelLoadUpdate(p->serverMotd.c_str());
            this->cc->levelLoadProgress(25);
            
            Logger::logf(PREFIX_NETWORK, "Sending login packet...\n");
            this->cc->levelLoadProgress(40);
            if (this->cc->userData != nullptr) {
                    LoginPacket* loginPacket = new LoginPacket(this->cc->userData->username, this->cc->userData->sessionid); // тут так-же ещё внутри отправляется версия протокола, главное не забывать менять, хд.
                    client->sendPacket(loginPacket);
                } else {
                    Logger::logf(PREFIX_WARNING, "User is null, sending guest login\n");
                    LoginPacket* loginPacket = new LoginPacket("", ""); 
                    client->sendPacket(loginPacket);
                }
            break;
        }

        case PacketType::LOGIN_RESPONSE: {
            LoginResponsePacket* p = static_cast<LoginResponsePacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Login successful! Server assigned name: %s\n", p->username.c_str());
            this->cc->playerId = p->playerId;
            this->client->loggedIn = true;
            if (this->cc->userData == nullptr) {
                this->cc->userData = new Data(p->username, ""); 
            } else {
                this->cc->userData->username = p->username;
            }
            this->cc->levelLoadProgress(60);
            RequestLevelDataPacket* requestLevel = new RequestLevelDataPacket();
            this->client->sendPacket(requestLevel);
            break;
        }

        case PacketType::LEVEL_DATA: {
            this->cc->levelLoadProgress(80);
            LevelLoadPacket* p = static_cast<LevelLoadPacket*>(packet);
            
            this->cc->level->isRemote = true;
            std::vector<uint8_t> levelData = this->cc->levelIO->decompressGzip(p->compressedData.data(), p->compressedData.size());
            this->cc->level->setData(p->width, p->depth, p->height, levelData); // fff
            this->cc->level->addEntity(this->cc->player);
            this->cc->level->player = this->cc->player;

            this->cc->levelLoadProgress(100);
            this->cc->canRender = true;
        
            Logger::logf(PREFIX_NETWORK, "Level data received and processed.\n");
            RequestSpawnPositionPacket* requestPacket = new RequestSpawnPositionPacket();
            this->client->sendPacket(requestPacket);
            break;
        }
            
        case PacketType::BLOCK_UPDATE: {
            BlockUpdatePacket* blockPacket = static_cast<BlockUpdatePacket*>(packet);
            this->cc->level->setTile(blockPacket->x, blockPacket->y, blockPacket->z, blockPacket->blockType);
            // Logger::logf(PREFIX_NETWORK, "Block updated at %i, %i, %i\n", blockPacket->x, blockPacket->y, blockPacket->z);
            break;
        }

        case PacketType::PLAYER_SPAWN: {
            SpawnPlayerPacket* p = static_cast<SpawnPlayerPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Spawning player %s (ID: %d)\n", p->username.c_str(), p->playerId);
            
            NetworkPlayer* new_player = new NetworkPlayer(this->cc->level, p->playerId, p->username, p->x, p->y, p->z, p->yaw, p->pitch);
            Logger::logf(PREFIX_NETWORK, "Username in player: %s", new_player->username.c_str());
            this->cc->level->networkPlayers[p->playerId] = new_player;
            break;
        }

        case PacketType::PLAYER_POSITION: {
            PositionPacket* p = static_cast<PositionPacket*>(packet);
            
            if (this->cc->player != nullptr && p->playerId == this->cc->playerId && p->type == 1) {
                this->cc->player->setPos(p->x, p->y, p->z);
                this->cc->player->yRot = p->yaw;
                this->cc->player->xRot = p->pitch;
                Logger::logf(PREFIX_NETWORK, "Teleported by server to %f, %f, %f\n", p->x, p->y, p->z);
            } 
            else {
                auto it = this->cc->level->networkPlayers.find(p->playerId);
                if (it != this->cc->level->networkPlayers.end()) {
                    it->second->queue(p->x, p->y, p->z, p->yaw, p->pitch);
                }
            }
            break;
        }

        case PacketType::PLAYER_DESPAWN: {
            DespawnPlayerPacket* p = static_cast<DespawnPlayerPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Despawning player (ID: %d)\n", p->playerId);

            this->cc->netData->removePlayer(p->playerId);

            
            break;
        }

        case PacketType::SET_SPAWN_POSITION: {
            SetSpawnPositionPacket* p = static_cast<SetSpawnPositionPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Received spawn position: %d, %d, %d\n", p->x, p->y, p->z);
            
            this->cc->level->setSpawnPos(p->x, p->y, p->z, p->yaw);
            
            this->cc->player->resetPos();
            Logger::logf(PREFIX_NETWORK, "New spawn pos: %d, %d, %d\n", this->cc->level->xSpawn, this->cc->level->ySpawn, this->cc->level->zSpawn);
            break;
        }

        case PacketType::SERVER_CHAT_MESSAGE: {
            ChatMessagePacket* p = static_cast<ChatMessagePacket*>(packet);
            this->cc->chatGui->addMessage(p->message);
            break;
        }
            
        // TODO: Other packets
        
        default:
            Logger::logf(PREFIX_WARNING, "Unknown packet type: %d\n", static_cast<int>(packet->getType()));
    }
}