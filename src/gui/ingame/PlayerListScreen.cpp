#include "gui/ingame/PlayerListScreen.hpp"
#include "CrossCraft.hpp" 
#include "level/Level.hpp"
#include "net/NetworkPlayer.hpp"

PlayerListScreen::PlayerListScreen() {
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
        int currentY = y0 + 24;
        int columnX = x0 + 10;
        
        drawString(this->cc->user->username.c_str(), columnX, currentY, 0xFFFFFF);
        currentY += 12;

        for (auto const& [id, netPlayer] : this->cc->level->networkPlayers) {
            
            if (netPlayer != nullptr) {
                drawString(netPlayer->username.c_str(), columnX, currentY, 0xFFFFFF);
                currentY += 12;
                
                if (currentY > y1 - 10) {
                    currentY = y0 + 24;
                    columnX += 100;
                    
                    if (columnX > x1 - 50) break; 
                }
            }
        }
    }
}
