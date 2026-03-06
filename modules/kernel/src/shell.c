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

#include <alias.h>
#include <bmp.h>

char **get_args(char *string, size_t start, char separator, int count);

int last_pos;
void *logo_bmp = NULL;
static int recursion_depth = 0;
static int recursion_limit = 32;

char *shell(char *command) // runs commands, returns command result
{
    static void *ptr;
    char **args = NULL;
    char *result = NULL;

    if (++recursion_depth > recursion_limit)
    {
        printf("shell recursion limit\n");
        goto error_cleanup;
    }

    char *alias = get_alias(command);
    if (alias != NULL)
        command = alias;

    if (strcmp(command, "help") == 0)
    {
        printf(
            "### Default Commands ###\n"
            "about - show system info\n"
            "add(a, b) - returns a + b\n"
            "aliases - show variables)\n"
            "beep(length) - make a sound\n"
            "clear - clear screen\n"
            "cpuid - show CPU info\n"
            "echo(text) - print text to screen\n"
            "logo - show os logo\n"
            "lspci - show all pci devices\n"
            "lsusb - show all usb devices\n"
            "video - show display info\n"
            "### Debug ###\n"
            "error(text) - print error message an halt\n"
            "pci(bus, slot, func) - show pci info\n"
            "setrecursionlimit(limit) - set shell recursion limit\n"
            "malloc(size) - allocate size of memory\n"
            "free - free last memory allocation\n"
            "exit - qemu shutdown\n"
            "### Cursor ###\n"
            "setpos(x, y) - set position of cursor\n"
            "retpos - set position to previous\n"
            "getpos - return position of cursor\n"
            "### Operators ###\n"
            "do(...) - run commands one after another separated by ';'\n"
            "rep(n, command) - repeat command n times\n"
            "var name=command - simplify some commands or create variable\n"
            "[command] - evaluate command\n"
            "### Info ###\n"
            "func(a, b, c) means the number and purpose of arguments.\n"
            "Arguments should be entered without parentheses and separated by a space.\n"
            "Command, that return something can be used in eval\n");
    }
    else if (strcmp(command, "about") == 0)
    {
        printf("ENAos\n\nSystem info:\n");
        shell("cpuid");
        shell("video");
        printf("Date of build is %s\n", __DATE__);
    }
    else if (strncmp(command, "add", 3) == 0)
    {
        args = get_args(command, 4, ' ', 2);
        if (args == NULL)
            goto error_cleanup;
        result = itoa(atoi(args[0]) + atoi(args[1]), NULL, 10);
    }
    else if (strcmp(command, "aliases") == 0)
    {
        list_aliases();
    }
    else if (strncmp(command, "var", 3) == 0)
    {
        args = get_args(command, 4, '=', 2);
        if (args == NULL)
            goto error_cleanup;
        set_alias(args[0], args[1]);
    }
    else if (strncmp(command, "beep", 4) == 0)
    {
        args = get_args(command, 5, ' ', 2);
        if (args == NULL)
            goto error_cleanup;
        beep(atoi(args[0]), atoi(args[1]));
    }
    else if (strcmp(command, "clear") == 0)
    {
        terminal_clear();
    }
    else if (strcmp(command, "cpuid") == 0)
    {
        printf("CPU: %s\n", cpu_get_brand_string());
    }
    else if (strncmp(command, "echo", 4) == 0)
    {
        args = get_args(command, 5, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        printf("%s\n", args[0]);
    }
    else if (strncmp(command, "error", 5) == 0)
    {
        args = get_args(command, 6, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        panic(args[0]);
    }
    else if (strncmp(command, "pci", 3) == 0)
    {
        args = get_args(command, 4, ' ', 3);
        if (args == NULL)
            goto error_cleanup;
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
    else if (strncmp(command, "rep", 3) == 0)
    {
        args = get_args(command, 4, ' ', 2);
        if (args == NULL)
            goto error_cleanup;
        int times = atoi(args[0]);
        for (int i = 0; i < times; i++)
            shell(args[1]);
    }
    else if (strncmp(command, "read", 4) == 0)
    {
        args = get_args(command, 5, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        char *data = kmalloc(PAGE_SIZE);
        void *start = data;
        ata_read((uint64_t *)data, atoi(args[0]), 8);
        while ((uint64_t)start + PAGE_SIZE != (uint64_t)data)
            printf("%c", *data++);
        printf("\n");

        ata_write(start, 0, 8);
        free(start);
    }
    else if (strncmp(command, "setrecursionlimit", 17) == 0)
    {
        args = get_args(command, 18, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        int lim = atoi(args[0]);
        if (lim < 1)
            printf("Can't set recursion limit: %d is less than 1", lim);
        else
            recursion_limit = lim;
    }
    else if (strncmp(command, "setpos", 6) == 0)
    {
        args = get_args(command, 7, ' ', 2);
        if (args == NULL)
            goto error_cleanup;
        last_pos = terminal_getpos();
        terminal_setpos(atoi(args[0]), atoi(args[1]));
    }
    else if (strcmp(command, "getpos") == 0)
    {
        result = itoa(terminal_getpos(), NULL, 10);
    }
    else if (strcmp(command, "retpos") == 0)
    {
        terminal_setpos(last_pos, 0);
    }
    else if (strncmp(command, "do", 2) == 0)
    {
        size_t count = strcount(&command[3], ';') + 1;
        args = get_args(command, 3, ';', count);
        if (args == NULL)
            goto error_cleanup;
        for (size_t i = 0; i < count; i++)
            shell(args[i]);
    }
    else if (strncmp(command, "int", 3) == 0)
    {
        args = get_args(command, 4, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        int interrupt = atoi(args[0]);
        __asm__("int $0" : "=r"(interrupt));
    }
    else if (strcmp(command, "lspci") == 0)
        show_pci_devices();
    else if (strcmp(command, "lsusb") == 0)
        show_usb_devices();
    else if (strcmp(command, "logo") == 0)
    {
        if (logo_bmp != NULL)
        {
            uint32_t scale_y = fb->common.framebuffer_height / terminal_height;

            BMPInfoHeader *info = get_bmp_info((const uint8_t *)logo_bmp);
            draw_bmp_at_position((const uint8_t *)logo_bmp,
                                 fb->common.framebuffer_width - info->width, scale_y * terminal_y);
            terminal_setpos(0, terminal_y + info->height / scale_y + 1);
        }
        else
        {
            printf("No logo loaded\n");
        }
    }
    else if (strcmp(command, "video") == 0)
        printf("Display %dx%d at 0x%lx\nTerminal width: %d height: %d\n",
               fb->common.framebuffer_width, fb->common.framebuffer_height, fb->common.framebuffer_addr,
               terminal_width, terminal_height);
    else if (strncmp(command, "malloc", 6) == 0)
    {
        ptr = kmalloc(atoi(&command[7]));
        printf("alloc at 0x%lx\n", (uint64_t)ptr);
    }
    else if (strcmp(command, "free") == 0)
    {
        free(ptr);
    }
    else if (recursion_depth == 1 && strcmp(command, "exit") != 0 && strcmp(command, "") != 0)
        printf("Unknown command: %s\n", command);
    else if (strcmp(command, "") != 0)
        result = command;

    recursion_depth = 0;
    free(args);
    return result;

error_cleanup:
    recursion_depth--;
    free(args);
    return "ERROR";
}
char **get_args(char *string, size_t start, char separator, int count)
{ // `arg` in evaluated
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

    int arg_variable = 0;
    for (size_t i = 0; i < length && arg_i < count; i++)
    {
        if (string[i] == '[')
            arg_variable++;
        if (string[i] == ']')
            arg_variable--;
        if (arg_i == count - 1)
        {
            args[arg_i++] = &string[prev];
            break;
        }
        if (arg_variable == 0 && (string[i] == separator || i == length - 1))
        {
            string[i] = '\0';
            args[arg_i++] = &string[prev];
            prev = i + 1;
        }
    }
    if (arg_i <= count - 1)
        goto args_error;

    for (int i = 0; i < count; i++)
    {
        char *cmd = args[i];
        if (cmd[0] != '[')
            continue;

        size_t len = strlen(cmd);
        cmd[len - 1] = '\0';
        args[i] = shell(&cmd[1]);
    }
    return args;

args_error:
    printf("Too few arguments. The command needs %d arguments.\n", count);
    free(args);
    return NULL;
}
