#include <fb_terminal.h>
#include <framebuffer.h>
#include <panic.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <mmu/pmm.h>

uint32_t fb_bg_color = 0x000000;
uint32_t fb_fg_color = 0xffffff;

void init_graphics_terminal()
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
    int pos = (terminal_x - 1) * CHARACTER_WIDTH + fb_width * terminal_y * CHARACTER_HEIGHT;
    for (int i = 0; i < CHARACTER_WIDTH * CHARACTER_HEIGHT; i++) // clear last row
        *(fb_video + pos + (i / 8 * fb_width) + (i % 8)) = fb_bg_color;
    terminal_x--;
    terminal_check_position();
}
void fb_clear()
{
    terminal_x = 0;
    terminal_y = 0;
    fb_fill_rect(0, 0, fb_width, fb_height, fb_bg_color);
    terminal_check_position();
}
void fb_scroll()
{
    for (uint32_t i = fb_width * CHARACTER_HEIGHT; i < fb_size; i++) // scroll all rows
        *(fb_video + i - fb_width * CHARACTER_HEIGHT) = *(fb_video + i);
    for (uint32_t i = (fb_size - fb_width * CHARACTER_HEIGHT); i < fb_size; i++) // clear last row
        *(fb_video + i) = fb_bg_color;
    terminal_y = terminal_height - 1;
}
void fb_set_color(uint32_t color)
{
    fb_fg_color = color;
}
void fb_write(char *string)
{
    for (; *string != '\0'; string++)
    {
        terminal_check_position();
        fb_put_char(*string, terminal_x * CHARACTER_WIDTH, terminal_y * CHARACTER_HEIGHT, fb_fg_color);
        terminal_x++;
    }
}
void fb_draw_cursor(int x, int y, uint32_t color)
{
    for (int px = 0; px < CHARACTER_WIDTH; px++)
    {
        fb_put_pixel(x * CHARACTER_WIDTH + px, y * CHARACTER_HEIGHT + CHARACTER_HEIGHT, color);
    }
}