#include <screen.h>
#include <string.h>
#include <memory.h>
#include <cursor.h>
#include <stdio.h>

void *video = (void *)0xb8000;
char black_screen[SCREEN_SIZE];

uint8_t x = 0;
uint8_t y = 0;
uint8_t color = SCREEN_COLOR_WHITE | SCREEN_COLOR_BLACK << 4;

void screen_init()
{
    memset(black_screen, ' ', SCREEN_SIZE);

    screen_clear();
    set_stdout(screen_write);
    set_getpos(get_position);
    set_setpos(set_position);
    set_clear(screen_clear);

    terminal_x = 80;
    terminal_y = 25;

    enable_cursor();
}
int get_position()
{
    return x + y * SCREEN_SIZE_X;
}
void scroll()
{
    for (int i = SCREEN_SIZE_X * 2; i < SCREEN_SIZE * 2; i++)
        *((char *)video + i - SCREEN_SIZE_X * 2) = *((char *)video + i);
    for (int i = (SCREEN_SIZE - SCREEN_SIZE_X); i < SCREEN_SIZE; i++)
        *((char *)video + i * 2) = 0;
    y = SCREEN_SIZE_Y - 1;
}
void checkPosition()
{
    if (x > SCREEN_SIZE_X)
    {
        y += x / SCREEN_SIZE_X;
        x = x % SCREEN_SIZE_X;
    }
    if (y >= SCREEN_SIZE_Y)
        scroll();
    update_cursor(x, y);
}
void set_position_to(int position)
{
    x = position;
    checkPosition();
}
void set_position(int collumn, int row)
{
    x = collumn;
    y = row;
    checkPosition();
}
void screen_clear()
{
    x = 0;
    y = 0;
    strext(video, black_screen, color);
    update_cursor(x, y);
}
void screen_set_color(uint8_t foreground, uint8_t background)
{
    color = foreground | background << 4;
}
void screen_write(char *string)
{
    strext((video + get_position() * 2), string, color);
    x += strlen(string);
    checkPosition();
}
