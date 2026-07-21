#include <shell.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <cpuid.h>
#include <framebuffer.h>
#include <pci.h>
#include <panic.h>
#include <pcspeaker.h>
#include <mmu/pmm.h>
#include <mmu/vmm.h>

#include <keyboard.h>
#include <drivers/ata.h>

#include <alias.h>
#include <bmp.h>

int last_pos;
void *logo_bmp = NULL;

void shell()
{
    char *input_buffer = kmalloc(4096);
    int input_len = 0;
    int bracket_depth = 0;

    while (true)
    {
        char line_buffer[256];
        memset(line_buffer, 0, 256);

        printf(">");
        for (int i = 0; bracket_depth > i; i++)
        {
            printf("\t");
        }

        terminal_gets(line_buffer);

        for (int i = 0; line_buffer[i] != '\0'; i++)
        {
            if (line_buffer[i] == '[')
                bracket_depth++;
            else if (line_buffer[i] == ']')
                bracket_depth--;
        }

        int line_len = strlen(line_buffer);
        if (input_len > 0)
        {
            if (input_len < 4095)
                input_buffer[input_len++] = '\n';
        }
        for (int i = 0; i < line_len; i++)
        {
            if (input_len < 4095)
                input_buffer[input_len++] = line_buffer[i];
        }
        input_buffer[input_len] = '\0';

        if (bracket_depth == 0)
        {
            char *res = evaluate(input_buffer);
            if (res != NULL && strcmp(res, "exit") == 0)
            {
                free(res);
                break;
            }
            if (res != NULL)
            {
                free(res);
            }
            input_len = 0;
            input_buffer[0] = '\0';
        }
    }
    if (input_buffer)
    {
        free(input_buffer);
    }
}