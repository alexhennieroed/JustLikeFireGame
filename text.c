#include "text.h"
#include "myLib.h"
#include "gameLogic.h"

void drawChar(int row, int col, char ch, u16 color) {
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 6; c++) {
            if(fontdata_6x8[OFFSET(r, c, 6) + (ch * 48)]) {
                setPixel(row+r, col+c, color);
            }
        }
    }
}

void drawString(int row, int col, char* str, u16 color) {
    int r = row;
    int c = col;
    while(*str) {
        if ((*str) == 0xa) {
            c = col + 6;
            r += 9;
            str++;
        } else {
            drawChar(r, c, *str++, color);
            c += 6;
        }
    }
}
