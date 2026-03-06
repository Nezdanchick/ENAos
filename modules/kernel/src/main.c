#include <init.h>
#include <shell.h>
#include <alias.h>
#include <history.h>
#include <gcursor.h>

void kmain()
{
    terminal_clear();
    shell("do logo;setpos 0 0;about;echo Welcome to ENAos!;echo Type 'help' for a list of commands");

    init_alias();
    init_history();

    timer_add_callback(gcursor_update, 20);

    char *input_buffer = kmalloc(0x100);

    while (true)
    {
        memset(input_buffer, 0, 256);
        printf("/>");
        terminal_gets(input_buffer);
        if (strcmp(shell(input_buffer), "exit") == 0)
            break;
    }
    printf("Goodbye!\n");
    free(input_buffer);
    outw(0x604, 0x2000); // qemu shutdown
}