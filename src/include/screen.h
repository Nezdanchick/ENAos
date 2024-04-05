#pragma once

#include <stdint.h>
#include <stddef.h>

#define SCREEN_SIZE_X 80
#define SCREEN_SIZE_Y 25
#define SCREEN_SIZE SCREEN_SIZE_X * SCREEN_SIZE_Y


enum uint8_t {
    SCREEN_COLOR_BLACK,
    SCREEN_COLOR_BLUE,
    SCREEN_COLOR_GREEN,
    SCREEN_COLOR_CYAN,
    SCREEN_COLOR_RED,
    SCREEN_COLOR_MAGENTA,
    SCREEN_COLOR_BROWN,
    SCREEN_COLOR_LIGHT_GRAY,
    SCREEN_COLOR_DARK_GRAY,
    SCREEN_COLOR_LIGHT_BLUE,
    SCREEN_COLOR_LIGHT_GREEN,
    SCREEN_COLOR_LIGHT_CYAN,
    SCREEN_COLOR_LIGHT_RED,
    SCREEN_COLOR_PINK,
    SCREEN_COLOR_YELLOW,
    SCREEN_COLOR_WHITE
};

extern void screenClear();
extern void screenSetColor(uint8_t foreground, uint8_t background);
extern void screenWrite(char *string);
extern void screenWriteLine(char *string);

extern size_t getPosition();
extern void setPosition(int collumn, int row);
extern void setPositionTo(int position);
