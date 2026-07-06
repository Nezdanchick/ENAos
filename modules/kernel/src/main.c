#include <init.h>
#include <shell.h>
#include <alias.h>
#include <history.h>
#include <gcursor.h>

char *init_script = "do clear;logo;setpos 0 0;"
                    "about;echo Welcome to ENAos!;"
                    "echo Type 'help' for a list of commands;"
                    "echo Press Up Arrow to see init command;retpos";

void kmain()
{
    init_alias();
    init_history();

    history_add(init_script);
    char *res1 = shell(init_script);
    free(res1);

    timer_add_callback(gcursor_blink, 1000);

    char *input_buffer = kmalloc(0x100);

    while (true)
    {
        memset(input_buffer, 0, 256);
        printf("/>");
        terminal_gets(input_buffer);
        char *res = shell(input_buffer);
        if (res != NULL && strcmp(res, "exit") == 0) {
            free(res);
            break;
        }
        free(res);
    }
    printf("Goodbye!\n");
    free(input_buffer);
    outw(0x604, 0x2000); // qemu shutdown
}