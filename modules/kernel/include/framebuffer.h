#pragma once

#include <stdint.h>
#include <multiboot2.h>

extern uint32_t fb_get_width();
extern uint32_t fb_get_height();
extern uint32_t fb_get_pitch();
extern uint8_t* fb_get_buffer();
extern uint64_t fb_get_address();

extern void init_framebuffer(struct multiboot_tag_framebuffer *tagfb);
extern void fb_clear();
extern void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
extern void fb_put_char(char ch, uint32_t x, uint32_t y, uint32_t color);
extern void fb_put_string(const char *string, uint32_t x, uint32_t y, uint32_t color);
