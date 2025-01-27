#include <shell.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <cpuid.h>
#include <framebuffer.h>
#include <pci.h>
#include <panic.h>
#include <pcspeaker.h>
#include <memory.h>
#include <mmu/pmm.h>
#include <mmu/vmm.h>

#include <drivers/ata.h>

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

static size_t alias_index = 1;
static char *alias_array = NULL;
void init_alias()
{
    alias_array = kmalloc(PAGE_SIZE);
}
void set_alias(char *alias, char *command)
{
    strcpy(&alias_array[alias_index], alias);
    alias_index += strlen(alias);
    alias_array[++alias_index] = 0;
    strcpy(&alias_array[alias_index], command);
    alias_index += strlen(command);
    alias_array[++alias_index] = 0;
}
char *get_alias(char *command)
{
    size_t offset = 0;
    for (size_t i = 0; i < PAGE_SIZE; i++)
    {
        if (alias_array[i] == '\0' && alias_array[i + 1] != '\0')
        {
            offset = i + 1;
            char *alias = &alias_array[offset];

            if (strcmp(alias, command))
            {
                char *cmd = NULL;
                for (size_t i = offset + 1; i < PAGE_SIZE; i++)
                {
                    if (alias_array[i] == '\0')
                    {
                        offset = i + 1;
                        cmd = &alias_array[offset];
                        break;
                    }
                }
                return cmd == NULL ? command : cmd;
            }
        }
    }
    return command;
}
void del_alias(char *command)
{
    size_t offset = 0;
    for (size_t i = 0; i < PAGE_SIZE; i++)
    {
        if (alias_array[i] == '\0' && alias_array[i + 1] != '\0')
        {
            offset = i + 1;
            char *alias = &alias_array[offset];

            if (strcmp(alias, command))
            {
                char *cmd = NULL;
                for (size_t i = offset + 1; i < PAGE_SIZE; i++)
                {
                    if (alias_array[i] == '\0')
                    {
                        offset = i + 1;
                        cmd = &alias_array[offset];

                        memset(alias, 0, strlen(alias) + strlen(cmd) + 1);
                        break;
                    }
                }
            }
        }
    }
}
bool shell(char *command) // runs commands, returns exit code, 1 = exit
{                         // TODO make alias commands
    static void *ptr;
    char **args = NULL;

    if (alias_array == NULL)
        init_alias();

    command = get_alias(command);

    if (strcmp(command, "help"))
        printf(
            "### Commands ###\n"
            "about - show system info\n"
            "add(a, b) - do simple math\n"
            "alias name=command - simplify some commands\n"
            "beep(length) - make a sound\n"
            "clear - clear screen\n"
            "cpuid - show CPU info\n"
            "echo(text) - print text to screen\n"
            "error(text) - print error message an halt\n"
            "exit - qemu shutdown\n"
            "free - free last memory allocation\n"
            "pci(bus, slot, func) - show pci info\n"
            "rep(n, command) - repeat command n times\n"
            "logo - show os logo\n"
            "lspci - show all pci devices\n"
            "malloc(size) - allocate size of memory\n"
            "test - show test image on display\n"
            "video - show display info\n"
            "### Info ###\n"
            "func(a, b, c) means the number and purpose of arguments.\n"
            "Arguments should be entered without parentheses and separated by a space.\n");
    else if (strcmp(command, "about"))
    {
        shell("cpuid");
        shell("video");
        printf("Welcome to ENAos! Date of build is %s\n", __DATE__);
    }
    else if (strncmp(command, "add", 3))
    {
        args = get_args(command, 4, ' ', 2);
        if (args == NULL)
            return false;
        printf("%d\n", atoi(args[0]) + atoi(args[1]));
    }
    else if (strncmp(command, "alias", 5))
    {
        args = get_args(command, 6, '=', 2);
        if (args == NULL)
            return false;
        printf("alias set for \"%s\" to \"%s\"\n", args[0], args[1]);
        set_alias(args[0], args[1]);
    }
    else if (strncmp(command, "-alias", 6))
    {
        args = get_args(command, 7, ' ', 1);
        if (args == NULL)
            return false;
        printf("alias deleted for \"%s\"\n", args[0]);
        del_alias(args[0]);
    }
    else if (strncmp(command, "beep", 4))
    {
        args = get_args(command, 5, ' ', 1);
        if (args == NULL)
            return false;
        beep(800, atoi(args[0]));
    }
    else if (strcmp(command, "clear"))
        terminal_clear();
    else if (strcmp(command, "cpuid"))
        printf("CPU: %s\n", cpu_get_brand_string());
    else if (strncmp(command, "echo", 4))
    {
        args = get_args(command, 5, ' ', 1);
        if (args == NULL)
            return false;
        printf("%s\n", args[0]);
    }
    else if (strncmp(command, "error", 5))
        panic(&command[6]);
    else if (strcmp(command, "exit"))
        return 1;
    else if (strncmp(command, "pci", 3))
    {
        args = get_args(command, 4, ' ', 3);
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
    else if (strncmp(command, "rep", 3))
    {
        args = get_args(command, 4, ' ', 2);
        if (args == NULL)
            return false;
        int times = atoi(args[0]);
        for (int i = 0; i < times; i++)
        {
            printf("%s\n", args[1]);
            shell(args[1]);
        }
    }
    else if (strncmp(command, "read", 4))
    {
        args = get_args(command, 5, ' ', 1);
        if (args == NULL)
            return false;
        char *data = kmalloc(PAGE_SIZE);
        void *start = data;
        ata_read((uint64_t*)data, atoi(args[0]), 8);
        while ((uint64_t)start + PAGE_SIZE != (uint64_t)data)
            printf("%c", *data++);
        printf("\n");

        ata_write(start, 0, 8);
        free(start);
    }
    else if (strncmp(command, "int", 3))
    {
        args = get_args(command, 4, ' ', 1);
        if (args == NULL)
            return false;
        int interrupt = atoi(args[0]);
        asm("int $0" : "=r"(interrupt));
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
    else if (strcmp(command, "search"))
    {
        void *pmm_search_free_block(uint64_t size);
        printf("found: 0x%lx\n", pmm_search_free_block(PAGE_SIZE));
    }
    else if (strcmp(command, "video"))
        printf("Display %dx%d at 0x%lx\nTerminal width: %d height: %d\n",
               fb->common.framebuffer_width, fb->common.framebuffer_height, fb->common.framebuffer_addr,
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

    free(args);
    return false;
}
char **get_args(char *string, size_t start, char separator, int count)
{
    size_t length = strlen(string);
    size_t offset = sizeof(char *) * count;
    char **args = kmalloc(offset + length);

    if (length <= start)
        goto args_error;
    char *strcopy = (void *)((uint64_t)args + offset);
    memcpy(strcopy, string, length);
    string = &strcopy[start];
    int arg_i = 0;
    size_t prev = 0;
    for (size_t i = 0; i < length && arg_i < count; i++)
    {
        if (arg_i == count - 1)
        {
            args[arg_i++] = &string[prev];
            break;
        }
        if (string[i] == separator || i == length - 1)
        {
            string[i] = '\0';
            args[arg_i++] = &string[prev];
            prev = i + 1;
        }
    }
    if (arg_i <= count - 1)
        goto args_error;
    return args;

args_error:
    printf("Too few arguments. The command needs %d arguments.\n", count);
    free(args);
    return NULL;
}
