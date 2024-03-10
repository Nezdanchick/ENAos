#include <screen.h>
#include <string.h>

void *video = (void *)0xb8000;

uint8_t x = 0;
uint8_t y = 0;
uint8_t color = SCREEN_COLOR_WHITE | SCREEN_COLOR_BLACK << 4;

size_t getPosition()
{
    return x + y * SCREEN_SIZE_X;
}
void checkPosition()
{
    if (x > SCREEN_SIZE_X)
    {
        y += x / SCREEN_SIZE_X;
        x = x % SCREEN_SIZE_X;
    }
    if (y >= SCREEN_SIZE_Y)
    {
        for (int i = SCREEN_SIZE_X * 2; i < SCREEN_SIZE * 2; i++)
        {
            *((char*)video + i - SCREEN_SIZE_X * 2) = *((char*)video + i);
        }
        y = SCREEN_SIZE_Y - 1;
    }
}
void setPositionTo(int position)
{
    x = position;
    checkPosition();
}
void setPosition(int collumn, int row)
{
    x = collumn;
    y = row;
    checkPosition();
}
void screenClear()
{
    x = 0;
    y = 0;
    memset(video, 0, SCREEN_SIZE * 2);
}
void screenSetColor(uint8_t foreground, uint8_t background)
{
    color = foreground | background << 4;
}
void screenWrite(char *string)
{
    strext((video + getPosition() * 2), string, color);
    x += strlen(string);
    checkPosition();
}
void screenWriteLine(char *string)
{
    screenWrite(string);
    x = 0;
    y++;
    checkPosition();
}
