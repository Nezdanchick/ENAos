#include <stdbool.h>
#include <string.h>
#include <screen.h>
#include <pcspeaker.h>
#include <asm.h>
#include <interrupts.h>
#include <timer.h>
#include <keyboard.h>
#include <memory.h>
#include <stdio.h>

void main()
{
    screen_init();
    interrupts_init();
    timer_init();
    keyboard_init();
    asm("sti");

    char *loader = "/|\\-";
    char progressbar[60];
    memset(progressbar, '_', 60);
    progressbar[0] = '[';
    progressbar[strlen(progressbar) - 1] = ']';

    for (int i = 0; true; i++)
    {
        progressbar[i + 1] = '#';
        printf(" Loading... %s %c\r", progressbar, loader[i % strlen(loader)]);
        sleep_ms(100);

        if (progressbar[strlen(progressbar) - 2] == '#')
            break;
    }
    terminal_clear();
    printf("Welcome to nandos! \n");
    char input_buffer[128];
    while (true)
    {
        memset(input_buffer, 0, 128);
        printf("/>");
        gets(input_buffer);

        if (strcmp(input_buffer, "exit"))
            break;
        else if (strcmp(input_buffer, "help"))
            printf("Nobody will help you\n");
        else if (strcmp(input_buffer, "clear"))
            terminal_clear();
        else if (strcmp(input_buffer, "lol"))
            printf("Why?\n");
        else if (!strcmp(input_buffer, "\0"))
            printf("%s\n", input_buffer);
    }
    printf("Goodbye!\n");
}
