#include <stdlib.h>
#include <stdio.h>

#include "myLib.h"
#include "gameLogic.h"

unsigned short* videoBuffer = (unsigned short*) 0x6000000;
FIRE fire_arr[25];
FIRE old_fire[25];
int gameLevel;
PLAYER player;
PLAYER old_player;

//Random -1 or 1 helper function
int randNegPos() {
    int r = rand() % 2;
    if (r == 0) {
        return -1;
    } else if (r == 1) {
        return 1;
    }
    return 0;
}

//Wait to draw the image until the vsync
void waitForVblank() {
    while(*SCANLINECOUNTER > 160);
    while(*SCANLINECOUNTER < 160);
}

//Sets a pixel to a specific color
void setPixel(int row, int col, u16 color) {
    videoBuffer[OFFSET(row, col, 240)] = color;
}

//Draw a rectange with its upper left corner at (row, col)
void drawRectangle(int row, int col, int height, int width, volatile u16 color) {
    for(int r = row; r < (row + height); r++) {
	    REG_DMA3SAD = (u32)&color;
        REG_DMA3DAD = (u32)&videoBuffer[OFFSET(row + r, col, 240)];
        REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

//Fills the screen with a color
void fillScreen(u16 color) {
    volatile u16 colr = color;
	volatile DMA_CONTROLLER* dma = DMA + 3;
	dma->src = &colr;
	dma->dst = videoBuffer;
	dma->cnt = (240 * 160) | DMA_ON | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT;
}

//Draws an image
void drawImage3(int row, int col, int height, int width, const u16* image) {
    for(int r = row; r < row + height; r++) {
        volatile DMA_CONTROLLER* dma = DMA + 3;
	    dma->src = &image[OFFSET(r - row, 0, width)];
	    dma->dst = &videoBuffer[OFFSET(row + r, col, 240)];
	    dma->cnt = width | DMA_ON | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT;
    }
}

//Sets all the game's start values
void getItStartedHot() {
    player.row = 50;
    player.col = 50;
    player.rd = 0;
    player.cd = 0;
    player.color = BLUE;
    player.size = 5;
    FIRE* old_cur;
    FIRE* cur;
    for (int i = 0; i < 5; i++) {
        cur = fire_arr + i;
        old_cur = old_fire + i;
        cur->row = 15 + (rand() % 135);
        cur->col = 15 + (rand() % 220);
        cur->rd = randNegPos();
        cur->cd = randNegPos();
        cur->color = RED;
        cur->size = 2;
        *old_cur = *cur;
    }
}

//Increase the game difficulty
void levelUp() {
    FIRE* cur;
    switch(gameLevel) {
        case 1:
        //Does nothing; same as start state
        break;
        
        case 2:
        for (int i = 5; i < 10; i++) {
            cur = fire_arr + i;
            cur->row = 15 + (rand() % 135);
            cur->col = 15 + (rand() % 220);
            cur->rd = randNegPos();
            cur->cd = randNegPos();
            cur->color = RED_ORANGE;
            cur->size = 3;
        }
        break;
        
        case 3:
        for (int i = 10; i < 15; i++) {
            cur = fire_arr + i;
            cur->row = 15 + (rand() % 135);
            cur->col = 15 + (rand() % 220);
            cur->rd = randNegPos();
            cur->cd = randNegPos();
            cur->color = ORANGE;
            cur->size = 4;
        }
        break;
        
        case 4:
        for (int i = 15; i < 20; i++) {
            cur = fire_arr + i;
            cur->row = 15 + (rand() % 135);
            cur->col = 15 + (rand() % 220);
            cur->rd = randNegPos();
            cur->cd = randNegPos();
            cur->color = GOLD;
            cur->size = 5;
        }
        break;
        
        case 5:
        for (int i = 20; i < 25; i++) {
            cur = fire_arr + i;
            cur->row = 15 + (rand() % 135);
            cur->col = 15 + (rand() % 220);
            cur->rd = randNegPos();
            cur->cd = randNegPos();
            cur->color = YELLOW;
            cur->size = 6;
        }
        break;        
    }
}

//Updates the fire balls
void fireUpdate() {
    FIRE* cur;
    for (int i = 0; i < gameLevel * 5; i++) {
        cur = fire_arr + i;
        cur->row += cur->rd;
        cur->col += cur->cd;
        if ((cur->col + cur->size) > 239) {
            cur->col = 239 - cur->size;
            cur->cd = -(cur->cd);
        }
        if (cur->col < 0) {
            cur->col = 0;
            cur->cd = -(cur->cd);
        }
        if ((cur->row + cur->size) > 159) {
            cur->row = 159 - cur->size;
            cur->rd = -(cur->rd);
        }
        if (cur->row < 6) {
            cur->row = 6;
            cur->rd = -(cur->rd);
        }
    }
}

//Updates the player
void playerUpdate() {
    player.row += player.rd;
    player.col += player.cd;
    if ((player.col + player.size) > 239) {
        player.col = 239 - player.size;
        player.cd = -(player.cd);
    }
    if (player.col < 0) {
        player.col = 0;
        player.cd = -(player.cd);
    }
    if ((player.row + player.size) > 159) {
        player.row = 159 - player.size;
        player.rd = -(player.rd);
    }
    if (player.row < 6) {
        player.row = 6;
        player.rd = -(player.rd);
    }
}

//Checks for collisions between the player and the fire balls
//Returning 0 means ther is a collision
int checkCollision() {
    FIRE* cur;
    for (int i = 0; i < gameLevel * 5; i++) {
        cur = fire_arr + i;
        for (int j = 0; j < player.size; j++) {
            for (int k = 0; k < cur->size; k++) {
                if (((cur->col) + k) == (player.col + j) && ((cur->row) + k) == (player.row + j)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

//Checks the user's use of the controls
void checkControls() {
    if (KEY_DOWN_NOW(BUTTON_UP)) {
        player.rd = -1;
    } else if (KEY_DOWN_NOW(BUTTON_DOWN)) {
        player.rd = 1;
    } else {
        player.rd = 0;
    }
    if (KEY_DOWN_NOW(BUTTON_LEFT)) {
        player.cd = -1;
    } else if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
        player.cd = 1;
    } else {
        player.cd = 0;
    }
}

//Draws all of the entities
void drawEntities() {
    //Draw player
    drawRectangle(old_player.row, old_player.col, old_player.size, old_player.size, GREEN);
    drawRectangle(player.row, player.col, player.size, player.size, player.color);
    old_player = player;
    //Draw fire
    FIRE* old_cur;
    FIRE* cur;
    for (int i = 0; i < gameLevel * 5; i++) {
        cur = fire_arr + i;
        old_cur = old_fire + i;
        drawRectangle(old_cur->row, old_cur->col, old_cur->size, old_cur->size, GREEN);
        drawRectangle(cur->row, cur->col, cur->size, cur->size, cur->color);
        *old_cur = *cur;
    }
}

//The loop that does the game things
//Returns 0 if there is a collision (a.k.a. you died)
int gameLoop(int level) {
    if (level != gameLevel) {
        gameLevel = level;        
        levelUp();
    }
    fireUpdate();
    checkControls();
    playerUpdate();
    waitForVblank();
    drawEntities();
    return checkCollision();
}
