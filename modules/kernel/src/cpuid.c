#include <stdint.h>
#include <string.h>

#define INTEL_MAGIC 0x756e6547
#define AMD_MAGIC 0x68747541

#define cpuid(in, a, b, c, d) __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(in));

char *cpu_get_brand_string()
{
    uint32_t regs[12];
    static char str[sizeof(regs)+1];

    cpuid(0x80000000, regs[0], regs[1], regs[2], regs[3]);

    if (regs[0] < 0x80000004)
        return str;

    cpuid(0x80000002, regs[0], regs[1], regs[2], regs[3]);
    cpuid(0x80000003, regs[4], regs[5], regs[6], regs[7]);
    cpuid(0x80000004, regs[8], regs[9], regs[10], regs[11]);

    memcpy(str, regs, sizeof(regs));
    str[sizeof(regs)] = '\0';
    return str;
}