#pragma once

#include <stdint.h>

extern uint32_t fb_bg_color;
extern uint32_t fb_fg_color;
extern uint32_t *fb_video;
extern uint32_t fb_width;
extern uint32_t fb_height;
extern uint32_t fb_size;

extern void init_graphics_terminal();
extern void fb_backspace();
extern void fb_clear();
extern void fb_scroll();
extern void fb_write(char *string);
extern void fb_draw_cursor(int x, int y, uint32_t color);
