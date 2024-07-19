#include <fb_terminal.h>
#include <framebuffer.h>
#include <panic.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define CHARACTER_HEIGHT 10
#define CHARACTER_WIDTH 8

extern uint32_t *video_fb;
extern uint32_t width_fb;
extern uint32_t height_fb;
extern uint32_t size_fb;

uint32_t fb_bg_color = 0x0f0f0f;
uint32_t fb_terminal_color = -1; // white

void fb_backspace();
void fb_clear();
void fb_scroll();
void fb_write(char *string);

void fb_terminal_init()
{
    if (fb == NULL)
        panic("Kernel fault in fb terminal: init framebuffer first.");

    set_backspace(fb_backspace);
    set_clear(fb_clear);
    set_stdout(fb_write);
    // set_cursor_setpos(fb_set_position);
    set_scroll(fb_scroll);

    terminal_width = fb->common.framebuffer_width / CHARACTER_WIDTH;
    terminal_height = fb->common.framebuffer_height / CHARACTER_HEIGHT;
}
void fb_backspace()
{
    int pos = (terminal_x - 1) * CHARACTER_WIDTH + width_fb * terminal_y * CHARACTER_HEIGHT;
    for (int i = 0; i < CHARACTER_WIDTH * CHARACTER_HEIGHT; i++) // clear last row
        *(video_fb + pos + (i / 8 * width_fb) + (i % 8)) = fb_bg_color;
    terminal_x--;
    terminal_check_position();
}
void fb_clear()
{
    terminal_x = 0;
    terminal_y = 0;
    fb_fill_rect(0, 0, width_fb, height_fb, fb_bg_color);
    terminal_check_position();
}
void fb_scroll()
{
    for (uint32_t i = width_fb * CHARACTER_HEIGHT; i < size_fb; i++) // scroll all rows
        *(video_fb + i - width_fb * CHARACTER_HEIGHT) = *(video_fb + i);
    for (uint32_t i = (size_fb - width_fb * CHARACTER_HEIGHT); i < size_fb; i++) // clear last row
        *(video_fb + i) = fb_bg_color;
    terminal_y = terminal_height - 1;
}
void fb_set_color(uint32_t color)
{
    fb_terminal_color = color;
}
void fb_write(char *string)
{
    for (; *string != '\0'; string++)
    {
        fb_put_char(*string, terminal_x * CHARACTER_WIDTH, terminal_y * CHARACTER_HEIGHT, fb_terminal_color);
        terminal_x++;
        terminal_check_position();
    }
}
