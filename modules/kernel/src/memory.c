#include <memory.h>

void *memset(void *buffer, int value, size_t size)
{
    while (size-- > 0)
    {
        *(int *)buffer++ = value;
    }
    return buffer;
}
bool memcmp(const void *a, const void *b, uint32_t size)
{
    while (size-- > 0)
    {
        if (*(int *)a != *(int *)b)
            return false;
    }
    return true;
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