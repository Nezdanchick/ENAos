#include <screen.h>
#include <string.h>
#include <memory.h>
#include <cursor.h>
#include <stdio.h>

#define SCREEN_SIZE 2000

void *video_text = (void*)0xb8000;;
char black_screen[SCREEN_SIZE];

uint8_t color = SCREEN_COLOR_WHITE | SCREEN_COLOR_BLACK << 4;

void screen_backspace();
void screen_clear();
void screen_scroll();
void screen_write(char *string);

void screen_init()
{
    memset(black_screen, ' ', SCREEN_SIZE);

    screen_clear();
    set_backspace(screen_backspace);
    set_clear(screen_clear);
    set_scroll(screen_scroll);
    set_stdout(screen_write);
    set_cursor_setpos(cursor_set);

    terminal_width = 80;
    terminal_height = 25;

    cursor_init();
}
void screen_backspace()
{
    terminal_x--;
    terminal_check_position();
    printf(" ");
    terminal_x--;
    terminal_check_position();
}
void screen_clear()
{
    terminal_x = 0;
    terminal_y = 0;
    strext(video_text, black_screen, color);
    cursor_set(0, 0);
}
void screen_scroll()
{
    for (int i = terminal_width * 2; i < SCREEN_SIZE * 2; i++)  // scroll all rows
        *((char *)video_text + i - terminal_width * 2) = *((char *)video_text + i);
    for (int i = (SCREEN_SIZE - terminal_width); i < SCREEN_SIZE; i++)  // clear last row
        *((char *)video_text + i * 2) = 0;
    terminal_y = terminal_height - 1;
}
void screen_write(char *string)
{
    strext((video_text + terminal_getpos() * 2), string, color);
    terminal_x += strlen(string);
    terminal_check_position();
}
// additional
void screen_set_color(uint8_t foreground, uint8_t background)
{
    color = foreground | background << 4;
}
