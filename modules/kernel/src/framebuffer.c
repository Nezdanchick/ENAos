#include <framebuffer.h>
#include <mmu/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <font.h>
#include <fb_terminal.h>

typedef struct
{
    uint64_t phys_addr;
    uint8_t *buffer;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint64_t size;
} fb_context_t;

static fb_context_t fb_ctx = {0};

uint32_t fb_get_width()
{
    return fb_ctx.width;
}
uint32_t fb_get_height()
{
    return fb_ctx.height;
}
uint32_t fb_get_pitch()
{
    return fb_ctx.pitch;
}
uint8_t *fb_get_buffer()
{
    return fb_ctx.buffer;
}
uint64_t fb_get_address()
{
    return fb_ctx.phys_addr;
}

static inline void put_pixel_fast(uint32_t x, uint32_t y, uint32_t color)
{
    uint32_t *pixel = (uint32_t *)(fb_ctx.buffer + (y * fb_ctx.pitch) + (x * sizeof(uint32_t)));
    *pixel = color;
}

#define FB_VIRT_ADDR 0xFFFF800000000000ULL

void init_framebuffer(struct multiboot_tag_framebuffer *fbtag)
{
    if (!fbtag)
        return;

    fb_ctx.phys_addr = fbtag->common.framebuffer_addr;
    fb_ctx.width = fbtag->common.framebuffer_width;
    fb_ctx.height = fbtag->common.framebuffer_height;
    fb_ctx.pitch = fbtag->common.framebuffer_pitch;
    fb_ctx.size = (uint64_t)fb_ctx.pitch * fb_ctx.height;

    // Use a constant high virtual address
    fb_ctx.buffer = (uint8_t *)FB_VIRT_ADDR;
    recursive_map(fb_ctx.phys_addr, FB_VIRT_ADDR, fb_ctx.size);
}

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x >= fb_ctx.width || y >= fb_ctx.height)
        return;

    put_pixel_fast(x, y, color);
}

void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    if (x >= fb_ctx.width || y >= fb_ctx.height)
        return;

    uint32_t draw_w = (x + w > fb_ctx.width) ? (fb_ctx.width - x) : w;
    uint32_t draw_h = (y + h > fb_ctx.height) ? (fb_ctx.height - y) : h;

    for (uint32_t cy = 0; cy < draw_h; cy++)
    {
        uint32_t *row = (uint32_t *)(fb_ctx.buffer + ((y + cy) * fb_ctx.pitch));
        for (uint32_t cx = 0; cx < draw_w; cx++)
        {
            row[x + cx] = color;
        }
    }
}

void fb_put_char(char ch, uint32_t x, uint32_t y, uint32_t color)
{
    if (x >= fb_ctx.width || y >= fb_ctx.height)
        return;

    uint64_t glyph = font[(unsigned char)ch];
    uint32_t bit_idx = 0;

    for (uint32_t cy = 0; cy < CHARACTER_HEIGHT; cy++)
    {
        for (uint32_t cx = 0; cx < CHARACTER_WIDTH; cx++)
        {
            uint32_t draw_x = x + cx;
            uint32_t draw_y = y + cy;

            if ((glyph >> bit_idx++) & 1)
            {
                if (draw_x < fb_ctx.width && draw_y < fb_ctx.height)
                {
                    put_pixel_fast(draw_x, draw_y, color);
                }
            }
        }
    }
}
