#pragma once

#include <stdint.h>
#include <stddef.h>
#include <multiboot2.h>

extern struct multiboot_tag_framebuffer *fb;
extern void fb_init(struct multiboot_tag_framebuffer *tagfb);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void fb_put_string(char *string, int32_t x, int32_t y, uint32_t color);
