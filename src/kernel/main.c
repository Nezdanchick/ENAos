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
#include <pci.h>

char **get_args(char *string, int count, char separator)
{
    int arg_i = 0;
    bool found = false;
    char *args[count];
    for (int i = 0; string[i] != '\0' && arg_i < count; i++)
    {
        if (!found && string[i] != separator)
        {
            args[arg_i++] = &string[i];
            found = true;
        }
        if (string[i] == separator)
        {
            string[i] = '\0';
            found = false;
        }
    }
    return args;
}
void main()
{
    screen_init();
    interrupts_init();
    timer_init();
    keyboard_init();
    asm("sti");

    terminal_clear();
    printf("Welcome to nandos!\n");
    char input_buffer[128];
    while (true)
    {
        memset(input_buffer, 0, 128);
        printf("/>");
        gets(input_buffer);

        if (strcmp(input_buffer, "about"))
            printf("Welcome to nandos!\n");
        else if (strcmp(input_buffer, "help"))
            printf(
            "### Help ###\n"
            "about - show system info\n"
            "add(a, b) - do simple math\n"
            "beep - make a sound\n"
            "clear - clear screen\n"
            "echo - print text to screen\n"
            "exit - qemu shutdown\n"
            "pci(bus, slot, func) - show pci info\n"
            "lspci - show all pci devices\n"
            "### Info ###\n"
            "func(a, b, c) means the number and purpose of arguments.\n"
            "Arguments should be entered without parentheses and separated by a space.\n"
            );
        else if (strcmp(input_buffer, "lspci"))
            show_pci_devices();
        else if (strncmp(input_buffer, "pci", 3))
        {
            char **args = get_args(&input_buffer[4], 3, ' ');
            pci_device_t device;
            read_pci_config(atoi(args[0]), atoi(args[1]), atoi(args[2]), &device);
            printf("Vendor: 0x%x, Device: 0x%x, Class: 0x%x, Subclass: 0x%x, Prog: 0x%x, Header: 0x%x\n",
                   device.vendor_id, device.device_id, device.class_code, device.subclass, device.prog_if, device.header_type);
        }
        else if (strcmp(input_buffer, "clear"))
            terminal_clear();
        else if (strcmp(input_buffer, "beep"))
            printf("\b");
        else if (strncmp(input_buffer, "echo", 4))
            printf("%s\n", &input_buffer[6]);
        else if (strncmp(input_buffer, "add", 3))
        {
            char **args = get_args(&input_buffer[4], 2, ' ');
            printf("%d\n", atoi(args[0]) + atoi(args[1]));
        }
        else if (strcmp(input_buffer, "exit"))
            break;
        else if (!strcmp(input_buffer, ""))
            printf("Command \"%s\" not found. Try \"help\".\n", input_buffer);
    }
    printf("Goodbye!\n");
    outw(0x604, 0x2000);    // qemu shutdown
}
