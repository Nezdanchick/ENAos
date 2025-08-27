#pragma once

#include <stdint.h>
#include <multiboot2.h>

extern struct multiboot_tag_framebuffer *fb;

extern void fb_init(struct multiboot_tag_framebuffer *tagfb);
extern void fb_clear();
extern void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void fb_fill_rect(uint32_t x, uint32_t y, int32_t w, int32_t h, uint32_t color);
extern void fb_put_char(char ch, uint32_t x, uint32_t y, uint32_t color);
extern void fb_put_string(char *string, int32_t x, int32_t y, uint32_t color);
