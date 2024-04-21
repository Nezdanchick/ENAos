#include <string.h>

char *strcpy(char *destination, const char *source)
{
    while (*source != '\0')
        *destination++ = *source++;
    return destination;
}
bool strcmp(const char *a, const char *b)
{
    if (strlen(a) != strlen(b))
        return false;
    while (*a != '\0' && *b != '\0')
    {
        if (*a++ != *b++)
            return false;
    }
    return true;
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

size_t strlen(const char *string)
{
    size_t i = 0;
    while (*string++ != '\0')
        i++;
    return i;
}

char *itoa(size_t value, char *str, int base)
{
    char *rc;
    char *ptr;
    char *low;
    // Check for supported base.
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10)
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while (low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}
