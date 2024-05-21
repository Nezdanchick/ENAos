#include <init.h>

static char *args[512] = {0};
char **get_args(char *string, size_t start, char separator, int count)
{
    if (strlen(string) <= start)
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
    return args;

args_error:
    printf("Too few arguments. The command needs %d arguments.\n", count);
    return NULL;
}
void video_test()
{
    int h = fb->common.framebuffer_height;
    int w = fb->common.framebuffer_width;
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            put_pixel(i, j, (((w - i) * (h - j)) & (i * (h - j)) & ((w - i) * j)) | (i * j));
        }
    }
}
void main()
{
    printf("Welcome to nandos!\n");
    char input_buffer[128];
    static void *ptr;
    while (true)
    {
        memset(input_buffer, 0, 128);
        printf("/>");
        gets(input_buffer);

        if (strcmp(input_buffer, "help"))
            printf(
                "### Help ###\n"
                "about - show system info\n"
                "add(a, b) - do simple math\n"
                "beep(length) - make a sound\n"
                "clear - clear screen\n"
                "echo(text) - print text to screen\n"
                "error(text) - print error message an halt\n"
                "exit - qemu shutdown\n"
                "pci(bus, slot, func) - show pci info\n"
                "lspci - show all pci devices\n"
                "test - show test image on display\n"
                "video - show display info\n"
                "### Info ###\n"
                "func(a, b, c) means the number and purpose of arguments.\n"
                "Arguments should be entered without parentheses and separated by a space.\n");
        else if (strcmp(input_buffer, "about"))
            printf("Welcome to nandos!\n");
        else if (strncmp(input_buffer, "addr", 4))
            printf("Phys: 0x%x\n", addr_virt_to_phys(atoi(&input_buffer[5])));
        else if (strncmp(input_buffer, "add", 3))
        {
            char **args = get_args(input_buffer, 4, ' ', 2);
            if (args == NULL)
                continue;
            printf("%d\n", atoi(args[0]) + atoi(args[1]));
        }
        else if (strncmp(input_buffer, "beep", 4))
        {
            char **args = get_args(input_buffer, 5, ' ', 1);
            if (args == NULL)
                continue;
            beep(800, atoi(args[0]));
        }
        else if (strcmp(input_buffer, "clear"))
            terminal_clear();
        else if (strncmp(input_buffer, "echo", 4))
            printf("%s\n", &input_buffer[5]);
        else if (strcmp(input_buffer, "test"))
        {
            terminal_clear();
            video_test();
        }
        else if (strncmp(input_buffer, "error", 5))
            panic(&input_buffer[6]);
        else if (strcmp(input_buffer, "exit"))
            break;
        else if (strncmp(input_buffer, "pci", 3))
        {
            char **args = get_args(input_buffer, 4, ' ', 3);
            if (args == NULL)
                continue;
            pci_device_t device;
            read_pci_config(atoi(args[0]), atoi(args[1]), atoi(args[2]), &device);
            printf(
                "Vendor:   0x%x\n"
                "Device:   0x%x\n"
                "Class:    0x%x\n"
                "Subclass: 0x%x\n"
                "Prog:     0x%x\n"
                "Header:   0x%x\n",
                device.vendor_id, device.device_id, device.class_code, device.subclass, device.prog_if, device.header_type);
        }
        else if (strcmp(input_buffer, "lspci"))
            show_pci_devices();
        else if (strncmp(input_buffer, "malloc", 6))
        {
            ptr = malloc(atoi(&input_buffer[7]));
            memory_block_info(ptr);
        }
        else if (strcmp(input_buffer, "free"))
        {
            free(ptr);
        }
        else if (strcmp(input_buffer, "video"))
            printf("Display %dx%d at 0x%x\n",
                   fb->common.framebuffer_width, fb->common.framebuffer_height, fb->common.framebuffer_addr);
        else if (!strcmp(input_buffer, ""))
            printf("Command '%s' not found. Try 'help'\n", input_buffer);
    }
    printf("Goodbye!\n");
    outw(0x604, 0x2000); // qemu shutdown
}
