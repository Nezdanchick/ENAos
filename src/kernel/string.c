#include <string.h>
#include <math.h>

const char NUMERIC_STRING[] = "0123456789abcdefghijklmnopqrstuvwxyz";

char *strcpy(char *destination, const char *source)
{
    while (*source != '\0')
        *destination++ = *source++;
    return destination;
}
bool strncmp(const char *a, const char *b, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (*a++ != *b++)
            return false;
    }
    return true;
}
bool strcmp(const char *a, const char *b)
{
    size_t length = strlen(a);
    if (length != strlen(b))
        return false;
    return strncmp(a, b, length);
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

char *itoa(int value, char *str, int base)
{
    char *result;
    char *ptr;
    char *start;
    // Check for supported base.
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    result = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10)
    {
        *ptr++ = '-';
        value = -value;
    }
    start = ptr;
    do
    {
        *ptr++ = NUMERIC_STRING[abs(value % base)];
        value /= base;
    } while (value);
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
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