#include <stdio.h>

#include "myLib.h"
#include "gameLogic.h"
#include "text.h"
#include "pinkJustLikeFire.h"
#include "consumedByFire.h"
#include "terror.h"

int calcTime(int cur_frames);

enum STATE {
    TITLE_D1,
    TITLE_D2,
    TITLE_NODRAW,
    INGAME_BG,
    INGAME,
    GAMEOVER,
    GAMEOVER_NODRAW
};

int main() {

    REG_DISPCNT = MODE3 | BG2_ENABLE;
    
    enum STATE state = TITLE_D1;
    
    int level = 1;
    int old_level = 0;
    int frame_counter = 0;
    int time = 0;
    int old_time = -1;
    int changed = 1;
    
    while(1) {
        waitForVblank();
        switch (state) {
            case TITLE_D1:
            drawImage3(0, 0, PINKJUSTLIKEFIRE_HEIGHT, PINKJUSTLIKEFIRE_WIDTH, pinkJustLikeFire);
            state = TITLE_D2;
            break;
            
            case TITLE_D2:
            drawString(125, 100, "Just Like Fire", WHITE);
            drawString(135, 100, "Press START to begin", GRAY);
            state = TITLE_NODRAW;
            break;
            
            case TITLE_NODRAW:
            if (KEY_DOWN_NOW(BUTTON_START)) {
                getItStartedHot();
                fillScreen(BLACK);
                state = INGAME_BG;
            }
            break;
            
            case INGAME_BG:                
            drawRectangle(5, 0, 150, 240, GREEN);
            state = INGAME;
            break;
        
            case INGAME:
            frame_counter++;
            time = calcTime(frame_counter);
            if (time != old_time) {
                changed = 0;
            }
            if (time % 30 == 0 && time != 0 && !changed && level < 5) {
                level++;
                changed = 1;
            }
            waitForVblank();
            char buffer[100];
            if (old_time != time || old_level != level) {
                drawRectangle(0, 0, 10, 240, BLACK);
                drawImage3(20, 60, TERROR_HEIGHT, TERROR_WIDTH, terror);
                waitForVblank();
                sprintf(buffer, "Level %d     Time: %d", level, time);
                drawString(1, 0, buffer, WHITE);
                old_time = time;
                old_level = level;
            }
            if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                state = TITLE_D1;
            }
            if (!gameLoop(level)) {
                state = GAMEOVER;
            }
            break;
        
            case GAMEOVER:
            drawImage3(0, 0, CONSUMEDBYFIRE_HEIGHT, CONSUMEDBYFIRE_WIDTH, consumedByFire);
            drawString(150, 5, "GAME OVER      Press SELECT to restart", GRAY);
            state = GAMEOVER_NODRAW;
            break;
            
            case GAMEOVER_NODRAW:
            if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                state = TITLE_D1;
            }
            break;
        }
    }
}

//Calculates the time based on the number of frames
int calcTime(int cur_frames) {
    return (cur_frames / 60);
}
