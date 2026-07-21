#include <string.h>
#include <stdint.h>
#include <math.h>

const char NUMERIC_STRING[] = "0123456789abcdefghijklmnopqrstuvwxyz";

char *strncpy(char *destination, const char *source, size_t len)
{
    char *start = destination;

    while (len > 0 && *source != '\0')
    {
        *destination++ = *source++;
        len--;
    }
    while (len-- > 0)
    {
        *destination++ = '\0';
    }

    return start;
}
char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while (*src != '\0')
        *d++ = *src++;
    *d = '\0';
    return dest;
}
int strncmp(const char *a, const char *b, size_t n)
{
    if (n == 0)
        return 0;

    while (--n && *a && (*a == *b))
    {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}
int strcmp(const char *a, const char *b)
{
    while (*a && (*a == *b))
    {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}
char *strcat(char *dest, const char *src)
{
    char *ptr = dest + strlen(dest);
    while (*src != '\0')
    {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}
char *strchr(const char *s, int c)
{
    while (*s != (char)c) {
        if (!*s++) {
            return NULL;
        }
    }
    return (char *)s;
}
char *strstr(const char *haystack, const char *needle)
{
    if (*needle == '\0')
        return (char *)haystack;

    while (*haystack)
    {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }

        if (*n == '\0')
            return (char *)haystack;

        haystack++;
    }

    return NULL;
}
char *strext(char *destination, char *source, char attribute)
{
    while (*source != '\0')
    {
        *destination++ = *source++;
        *destination++ = attribute;
    }
    return destination;
}
char *strtok(char *string, const char *separator)
{
    char *start = string;
    size_t length = strlen(separator);
    while (*string++)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (*string == separator[i])
            {
                *string++ = '\0';
                return start;
            }
        }
    }
    return NULL;
}
size_t strlen(const char *string)
{
    size_t i = 0;
    while (*string++ != '\0')
        i++;
    return i;
}

size_t strcount(const char *string, char character)
{
    size_t count = 0;
    while (*string++)
    {
        if (*(string - 1) == character)
            count++;
    }
    return count;
}

char *itoa(size_t value, char *str, int base)
{
    char *result;
    char *ptr;
    char *start;

    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    result = ptr = str;

    if ((int64_t)value < 0 && base == 10)
    {
        *ptr++ = '-';
        value = -value;
    }
    start = ptr;
    do
    {
        *ptr++ = NUMERIC_STRING[value % abs(base)];
        value /= base;
    } while (value);

    *ptr-- = '\0';

    while (start < ptr)
    {
        char tmp = *start;
        *start++ = *ptr;
        *ptr-- = tmp;
    }
    return result;
}
int atoi(const char *string)
{
    size_t i = 0;
    int result = 0;
    if (*string == '-')
        i++;
    for (; string[i] >= '0' && string[i] <= '9'; ++i)
        result = 10 * result + (string[i] - '0');
    if (*string == '-')
        result *= -1;
    return result;
}
