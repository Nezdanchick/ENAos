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