#include <string.h>

#define COMPILER_BARRIER() __asm__ volatile("" : : : "memory")

void *memset(void *buffer, int value, size_t size)
{
    unsigned char *p = buffer;
    while (size-- > 0)
    {
        *p++ = (unsigned char)value;
    }
    return buffer;
}
int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;

    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
            return p1[i] - p2[i];
    }
    return 0;
}
void *memcpy(void *dest, const void *src, size_t count)
{
    if (dest == src)
        return dest;

    uint8_t *destination = (uint8_t *)dest;
    uint8_t *source = (uint8_t *)src;
    for (size_t i = 0; i < count; i++)
        *destination++ = *source++;
    return destination;
}
void *memmove(void *dest, const void *src, size_t n)
{
    if (dest == src)
        return dest;

    uint8_t *destination = (uint8_t *)dest;
    uint8_t *source = (uint8_t *)src;
    if (destination < source)
    {
        for (size_t i = 0; i < n; i++)
            destination[i] = source[i];
    }
    else
    {
        for (size_t i = n; i > 0; i--)
            destination[i - 1] = source[i - 1];
    }
    return dest;
}
