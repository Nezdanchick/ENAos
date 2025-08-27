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
    uint8_t *destination = (uint8_t *)dest;
    uint8_t *source = (uint8_t *)src;
    for (size_t i = 0; i < count; i++)
        *destination++ = *source++;
    return destination;
}
