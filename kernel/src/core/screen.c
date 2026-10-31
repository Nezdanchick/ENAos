#include "screen.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

static volatile uint16_t *video_text = (volatile uint16_t *)0xFFFFFFFF800B8000;
static const uint8_t color = 0x0F;

static size_t terminal_x = 0;
static size_t terminal_y = 0;

void screen_clear(void)
{
    terminal_x = 0;
    terminal_y = 0;
    for (size_t i = 0; i < SCREEN_SIZE; i++) {
        video_text[i] = (uint16_t)' ' | ((uint16_t)color << 8);
    }
}

static void screen_scroll(void)
{
    for (size_t i = 0; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
        video_text[i] = video_text[i + SCREEN_WIDTH];
    }
    for (size_t i = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i < SCREEN_SIZE; i++) {
        video_text[i] = (uint16_t)' ' | ((uint16_t)color << 8);
    }
    terminal_y = SCREEN_HEIGHT - 1;
}

void screen_putc(char c)
{
    if (c == '\n') {
        terminal_x = 0;
        terminal_y++;
    } else if (c == '\r') {
        terminal_x = 0;
    } else {
        video_text[terminal_y * SCREEN_WIDTH + terminal_x] = (uint16_t)c | ((uint16_t)color << 8);
        terminal_x++;
        if (terminal_x >= SCREEN_WIDTH) {
            terminal_x = 0;
            terminal_y++;
        }
    }

    if (terminal_y >= SCREEN_HEIGHT) {
        screen_scroll();
    }
}

void screen_write(char *string)
{
    while (*string != '\0') {
        screen_putc(*string++);
    }
}

void screen_init(void)
{
    screen_clear();
}
