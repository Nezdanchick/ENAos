#include <fb_terminal.h>
#include <framebuffer.h>
#include <font.h>
#include <gcursor.h>
#include <panic.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <mmu/pmm.h>

uint32_t fb_bg_color = 0x000000;
uint32_t fb_fg_color = 0xffffff;

void init_graphics_terminal()
{
    if (fb_get_buffer() == NULL)
        panic("Kernel fault in fb terminal: init framebuffer first.");

    set_backspace(fb_backspace);
    set_clear(fb_clear);
    set_stdout(fb_write);
    set_scroll(fb_scroll);

    terminal_width = fb_get_width() / CHARACTER_WIDTH;
    terminal_height = fb_get_height() / CHARACTER_HEIGHT;
}

void fb_backspace()
{
    fb_fill_rect(terminal_x-- * CHARACTER_WIDTH, terminal_y * CHARACTER_HEIGHT, CHARACTER_WIDTH, CHARACTER_HEIGHT, fb_bg_color);
    terminal_check_position();
}

void fb_clear()
{
    terminal_x = 0;
    terminal_y = 0;
    fb_fill_rect(0, 0, fb_get_width(), fb_get_height(), fb_bg_color);
    terminal_check_position();
}

void fb_scroll()
{
    gcursor_hide();
    gcursor_update();

    uint32_t pitch = fb_get_pitch();
    uint32_t height = fb_get_height();
    uint8_t *buffer = fb_get_buffer();

    uint32_t scroll_offset = pitch * CHARACTER_HEIGHT;
    uint32_t copy_size = (height * pitch) - scroll_offset;

    memmove(buffer, buffer + scroll_offset, copy_size);
    fb_fill_rect(0, height - CHARACTER_HEIGHT, fb_get_width(), CHARACTER_HEIGHT, fb_bg_color);

    terminal_y = terminal_height - 2;

    gcursor_show();
    gcursor_update();
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
    fb_fill_rect(x * CHARACTER_WIDTH, y * CHARACTER_HEIGHT + CHARACTER_HEIGHT, CHARACTER_WIDTH, 2, color);
}