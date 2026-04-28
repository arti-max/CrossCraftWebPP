#include "gui/ingame/PlayerListScreen.hpp"
#include "CrossCraft.hpp" 
#include "level/Level.hpp"
#include "net/NetworkPlayer.hpp"
#include "util/Logger.hpp"

PlayerListScreen::PlayerListScreen() {
}

void PlayerListScreen::mouseClicked(int mx, int my, int mbtn) {
    if (mbtn == 0) {
        Logger::logf(PREFIX_DEBUG, "PLAYEr LIST SCREEN: clicked! %s\n", this->selectedUsername.c_str());

        if (this->selectedUsername != "") {
            Logger::logf(PREFIX_DEBUG, "PLAYEr LIST SCREEN: not NULL!!!\n");
            ChatScreen* chat = (ChatScreen*)this->cc->screen;
            if (chat != nullptr) {
                Logger::logf(PREFIX_DEBUG, "PLAYEr LIST SCREEN: %s\n", this->selectedUsername.c_str());
                chat->addText(this->selectedUsername); 
            }
        }
    }
}

void PlayerListScreen::render(int xMouse, int yMouse) {
    int x = this->width / 2;
    int y = this->height / 2;
    int w = 250;
    int h = 150;

    int x0 = x - w / 2;
    int y0 = y - h / 2;
    int x1 = x + w / 2;
    int y1 = y + h / 2;

    fill(x0, y0, x1, y1, 0x80000000);
    drawCenteredString("Connected players:", x, y0 + 4, 0xFFFFFF);

    if (this->cc->level != nullptr) {
        std::vector<std::string> usernames;
        usernames.push_back(this->cc->userData->username);
        for (auto const& [id, netPlayer] : this->cc->level->networkPlayers) {
            if (netPlayer != nullptr) {
                usernames.push_back(netPlayer->username);
            }
        }

        const int startX = x0 + 10;
        const int startY = y0 + 24;
        const int colWidth = 100;
        const int rowHeight = 12;
        const int maxCols = 2;

        bool selectedName = false;

        for (int i = 0; i < usernames.size(); ++i) {
            int col = i % maxCols;
            int row = i / maxCols;
            int drawX = startX + col * colWidth;
            int drawY = startY + row * rowHeight;

            bool mouseInName = xMouse >= drawX && yMouse >= drawY && xMouse < drawX + cc->font->width(usernames[i]) && yMouse < drawY + 10;
            if (mouseInName) {
                drawX+=10;
                this->selectedUsername = usernames[i];
                selectedName = true;
                //Logger::logf(PREFIX_DE BUG, "PLAYER LIST SCREEN (RENDER): %s, Selected: %s, index: %i\n", usernames[i].c_str(), this->selectedUsername.c_str(), i);
            } else {
                if (!selectedName) {
                    this->selectedUsername = "";
                }
            }

            if (drawY + rowHeight > y1 - 10) break;

            drawString(usernames[i].c_str(), drawX, drawY, 0xFFFFFF);
        }
    }
}