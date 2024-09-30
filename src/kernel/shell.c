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

char *logo = ""
             "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n"
             "::::::::::::::::.+:::::::####::##::#::::#::::#####::#####:::::::::::::::::::::::\n"
             ":::::::::::::::.x+.::::::#;;;::#:#:#:::#:#:::#:::#::#:::::::::::::::::::::::::::\n"
             ":::::::::::::::+xx.::::::####::#:#:#::#:::#::#:::#::#####:::::::::::::::::::::::\n"
             ":::::::::::::::xxx:::::::#:::::#:#:#;:#####::#:::#::::::#:::::::::::::::::::::::\n"
             ":::::::::::::.xxxx:::::::####::#::##::#:::#::#####::#####::::::::::::;+X$&&&&;::\n"
             ":::::::::::::;xxxx::::::::::::::::::::::::::::::::..:;+XX&&&&&&&&&&&&&&&&&&&&:::\n"
             "::::::::::::.xxxxx;:::::::::::::::....::;:$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$.::\n"
             ":::::::::::.+xxxxx+....::;++xxxxxxxxxxxxx+&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&;:::\n"
             ":::::::::::xxxxxxxxxxxxxxxxxxxxxxxxxxxxx+&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$::::\n"
             ":::::::::.+xxxxxxx::+xxxxxxxxxxxxxxxxxxx+&&&&&&&&&&&&&&&X:.     .;x&&&&&&&&+::::\n"
             ":::::::::;xxxxxx+&&    .:xxxxxxxxxxxxxxx;&&&&&&&&&&&x&&&&:            :&&&&.::::\n"
             "::::::::.xxxxx+&&&&         :+xxxxxxxxxxX&&&&&&&&&X&&&&&&X               +$:::::\n"
             "::::::::+xxxxX&&&&&;            .+xxxxxx&&&&&&&&X&&&&&&&&&                ..::::\n"
             "::::::::xxx;&&&&&&&X                .+xx&&&&&&&&&&&&&&&&&&                .:::::\n"
             ":::::::xxxX&&&&&&&&&                 +xx&&&&&X&&&&&&&&&&&&                .:::::\n"
             ":::::.+x+&&&&&&&&&&&.              +xxxx&&&&X&&&&&&&&&&&&&  ..:;xX$$&&&&&$::::::\n"
             ":::::+x;&&&&&&&&&&&&+            .xxxxx+&&&$&&&&Xx$&&&&&&&&&&&&&&&&&&&&&&;::::::\n"
             "::::;x+$&&&&&&&&&&&&&           +xxxxxx:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&.::::::\n"
             ":::.xxxxx+X&&&&&&&&&&.        :xxxxxxxx+&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$:::::::\n"
             "xx;+xxxxxxxx:.&&&&&&&+      .+xxxxxxxxx&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&;:::::::\n"
             "xxxxxxxxxxx+  .++x&&&&     ;xxxxxxxxxxx&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&.:::::::\n"
             "xxxxxxxxxxx:;xxxxxxx+&.  .xxxxxxxxxxxx+&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$::::::::\n"
             "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&+::::::::\n"
             "xxxxxxx;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+&&&&&&&&&&&&&&&&&&&&&&&&&&&$:::::::::\n"
             "xx+:.:::xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+&&&&&&&&&&&&&x+&&&&&&&&&&&&::::::::::\n"
             ".::::::::xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+&&&&&&&xxxxxxx&&&&&&&&&&&&&.:::::::::\n"
             "::::::::.+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx&&&xxxxxxxxx+&&&&&&&&&&&&&&$::::::::::\n"
             "::::::::::xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+&&&&&&&&&&&&&&&;::::::::::\n"
             "::::::::::+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+$&&&&&&&&&&&&&&&&.::::::::::\n"
             ":::::::::::.:+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+$&&&&&&&&&&&&&&&&&&&.:::::::::::\n"
             ":::::::::::::::.:+xxxxxxxxxxxxxxxxxxxxxxxxxxxx+&&&&&&&&&&&&&&&&&&;:.::::::::::::\n"
             ":::::::::::::::::::.:+xxxxxxxxxxxxxxxxxxxxxx+X&&&&&&&&&&&&&&&$:.:::.::::::::::::\n"
             ":::::::::::::::::::::::.:+xxxxxxxxxxxxxxxxxx+&&&&&&&&&&&&&+.:::::::.::::::::::::\n"
             ":::::::::::::::::::::::::::.:+xxxxxxxxxxxxxx+&&&&&&&&&&;::::::::::.:::::::::::::\n"
             ":::::::::::::::::::::::::::::::..;xxxxxxxxxxx$&&&&&$:.::::::::::::.:::::::::::::\n"
             "::::::::::::::::::::::::::::::::::::..;xxxxxxX&&X:::::::::::::::::.::::::::::..:\n"
             "";

char **get_args(char *string, size_t start, char separator, int count);

void video_test()
{
    int h = fb->common.framebuffer_height;
    int w = fb->common.framebuffer_width;
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            fb_put_pixel(i, j, (((w - i) * (h - j)) & (i * (h - j)) & ((w - i) * j)) | (i * j));
        }
    }
}
extern uint64_t video_fb;
bool shell(char *command) // runs commands, returns exit code, 1 = exit
{
    static void *ptr;
    if (strcmp(command, "help"))
        printf(
            "### Commands ###\n"
            "about - show system info\n"
            "add(a, b) - do simple math\n"
            "beep(length) - make a sound\n"
            "clear - clear screen\n"
            "cpuid - show CPU info\n"
            "echo(text) - print text to screen\n"
            "error(text) - print error message an halt\n"
            "exit - qemu shutdown\n"
            "pci(bus, slot, func) - show pci info\n"
            "logo - show os logo\n"
            "lspci - show all pci devices\n"
            "test - show test image on display\n"
            "video - show display info\n"
            "### Info ###\n"
            "func(a, b, c) means the number and purpose of arguments.\n"
            "Arguments should be entered without parentheses and separated by a space.\n");
    else if (strcmp(command, "about"))
    {
        shell("logo");
        shell("cpuid");
        shell("video");
        printf("Welcome to ENAos! Date of build is %s\n", __DATE__);
    }
    else if (strncmp(command, "add", 3))
    {
        char **args = get_args(command, 4, ' ', 2);
        if (args == NULL)
            return false;
        printf("%d\n", atoi(args[0]) + atoi(args[1]));
    }
    else if (strncmp(command, "beep", 4))
    {
        char **args = get_args(command, 5, ' ', 1);
        if (args == NULL)
            return false;
        beep(800, atoi(args[0]));
    }
    else if (strcmp(command, "clear"))
        terminal_clear();
    else if (strcmp(command, "cpuid"))
        detect_cpu();
    else if (strncmp(command, "echo", 4))
        printf("%s\n", &command[5]);
    else if (strncmp(command, "error", 5))
        panic(&command[6]);
    else if (strcmp(command, "exit"))
        return 1;
    else if (strncmp(command, "pci", 3))
    {
        char **args = get_args(command, 4, ' ', 3);
        if (args == NULL)
            return false;
        pci_device_t device;
        pci_read_config(atoi(args[0]), atoi(args[1]), atoi(args[2]), &device);
        printf(
            "Vendor:   0x%x\n"
            "Device:   0x%x\n"
            "Class:    0x%x\n"
            "Subclass: 0x%x\n"
            "Prog:     0x%x\n"
            "Header:   0x%x\n",
            device.vendor_id, device.device_id, device.class_code, device.subclass, device.prog_if, device.header_type);
    }
    else if (strcmp(command, "lspci"))
        show_pci_devices();
    else if (strcmp(command, "logo"))
        printf(logo);
    else if (strcmp(command, "test"))
    {
        terminal_clear();
        video_test();
    }
    else if (strcmp(command, "video"))
        printf("Display %dx%d at 0x%lx\nTerminal width: %d height: %d\n",
               fb->common.framebuffer_width, fb->common.framebuffer_height, video_fb,
               terminal_width, terminal_height);
    else if (strncmp(command, "malloc", 6))
    {
        ptr = kmalloc(atoi(&command[7]));
        printf("alloc at 0x%lx\n", (uint64_t)ptr);
    }
    else if (strcmp(command, "free"))
        free(ptr);
    else if (!strcmp(command, ""))
        printf("Command '%s' not found. Try 'help'\n", command);
    return false;
}
char **get_args(char *string, size_t start, char separator, int count)
{
    size_t length = strlen(string);
    char **args = kmalloc(length);
    if (length <= start)
        goto args_error;
    string = &string[start];
    int arg_i = 0;
    bool found = false;
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
    if (arg_i <= count - 1)
        goto args_error;
    free(args);
    return args;

args_error:
    printf("Too few arguments. The command needs %d arguments.\n", count);
    return NULL;
}
