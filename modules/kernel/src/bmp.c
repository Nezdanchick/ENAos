#include <bmp.h>
#include <stddef.h>

BMPInfoHeader* get_bmp_info(const uint8_t* bmp_data)
{
    if (!bmp_data) return NULL;

    const BMPFileHeader* file_header = (const BMPFileHeader*)bmp_data;
    const BMPInfoHeader* info_header = (const BMPInfoHeader*)(bmp_data + sizeof(BMPFileHeader));

    if (file_header->type != 0x4D42) return NULL;
    return (BMPInfoHeader*)info_header;
}

void draw_bmp_at_position(const uint8_t* bmp_data, uint32_t x_offset, uint32_t y_offset) {
    if (!bmp_data) return;
    
    const BMPFileHeader* file_header = (const BMPFileHeader*)bmp_data;
    const BMPInfoHeader* info_header = (const BMPInfoHeader*)(bmp_data + sizeof(BMPFileHeader));
    
    if (file_header->type != 0x4D42) return;
    if (info_header->bits_per_pixel != 24) return;
    
    int32_t width = info_header->width;
    int32_t height = info_header->height;
    int is_top_down = (height < 0);
    
    if (is_top_down) {
        height = -height;
    }
    
    const uint8_t* pixel_data = bmp_data + file_header->offset;
    uint32_t row_size = ((width * 3 + 3) / 4) * 4;
    
    for (int32_t y = 0; y < height; y++) {
        const uint8_t* row_ptr;
        int32_t screen_y;
        
        if (is_top_down) {
            row_ptr = pixel_data + y * row_size;
            screen_y = y_offset + y;
        } else {
            row_ptr = pixel_data + (height - 1 - y) * row_size;
            screen_y = y_offset + y;
        }
        
        for (int32_t x = 0; x < width; x++) {
            const uint8_t* pixel_ptr = row_ptr + x * 3;
            uint8_t b = pixel_ptr[0];
            uint8_t g = pixel_ptr[1];
            uint8_t r = pixel_ptr[2];
            
            uint32_t color = (r << 16) | (g << 8) | b;
            fb_put_pixel(x_offset + x, screen_y, color);
        }
    }
    return;
}
