#include <shell.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <alias.h>
#include <cpuid.h>
#include <framebuffer.h>
#include <pci.h>
#include <panic.h>
#include <pcspeaker.h>
#include <keyboard.h>
#include <timer.h>
#include <fs/vfs.h>
#include <bmp.h>
#include <drivers/ata.h>
#include <mmu/pmm.h>

extern int last_pos;

static unsigned long int rng_next = 12345;
int rand()
{
    rng_next = rng_next * 1103515245 + 12345;
    return (unsigned int)(rng_next / 65536) % 32768;
}

char *execute_builtin(char *command)
{
    char **args = NULL;
    char *result = kmalloc(512);
    result[0] = '\0';

    if (strcmp(command, "help") == 0)
    {
        char *help_res = execute_builtin("cat sys/resourses/help.txt");
        if (help_res)
        {
            strcpy(result, help_res);
            free(help_res);
        }
    }
    else if (strcmp(command, "about") == 0)
    {
        printf("ENAos\n\nSystem info:\n");
        char *cpuid_res = evaluate("cpuid");
        if (cpuid_res)
            free(cpuid_res);
        char *video_res = evaluate("video");
        if (video_res)
            free(video_res);
        printf("Date of build is %s\n", __DATE__);
    }
    else if (strncmp(command, "add", 3) == 0)
    {
        args = get_args(command, 4, ' ', 2);
        if (args == NULL)
            goto error_cleanup;

        char *eval = eval_string(args[0]);
        int val0 = atoi(eval);
        free(eval);

        eval = eval_string(args[1]);
        int val1 = atoi(eval);
        free(eval);

        itoa(val0 + val1, result, 10);
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

        char *raw_name = args[0];
        char *val = args[1];

        char *eval_name = eval_string(raw_name);

        size_t rn_len = strlen(raw_name);
        if (rn_len >= 2 && raw_name[rn_len - 2] == '[' && raw_name[rn_len - 1] == ']')
        {
            char *evaluated_val = eval_string(val);
            int arr_idx = 0;
            char *token = evaluated_val;
            char *next_token = strchr(token, ' ');

            while (token != NULL && *token != '\0')
            {
                if (next_token)
                {
                    *next_token = '\0';
                    next_token++;
                }

                char var_name[64];
                int vnl = 0;
                for (int k = 0; eval_name[k] && vnl < 63; k++)
                    var_name[vnl++] = eval_name[k];
                char idx_str[10];
                itoa(arr_idx, idx_str, 10);
                for (int k = 0; idx_str[k] && vnl < 63; k++)
                    var_name[vnl++] = idx_str[k];
                var_name[vnl] = '\0';

                set_alias(var_name, token);

                arr_idx++;
                token = next_token;
                if (token)
                    next_token = strchr(token, ' ');
            }
            if (evaluated_val)
                free(evaluated_val);
        }
        else
        {
            char *val_to_set = NULL;
            size_t len = strlen(val);
            if (len >= 2 && val[0] == '[' && val[len - 1] == ']')
            {
                bool has_nl = false;
                for (size_t i = 0; i < len; i++)
                    if (val[i] == '\n')
                        has_nl = true;

                if (has_nl)
                    val_to_set = val;
                else
                    val_to_set = eval_string(val);
            }
            else
            {
                val_to_set = val;
            }

            set_alias(eval_name, val_to_set);

            if (val_to_set != val)
                free(val_to_set);
        }
        if (eval_name)
            free(eval_name);
    }
    else if (strncmp(command, "rand", 4) == 0)
    {
        args = get_args(command, 5, ' ', 2);
        if (args != NULL)
        {
            char *eval0 = eval_string(args[0]);
            char *eval1 = eval_string(args[1]);
            int from = atoi(eval0);
            int to = atoi(eval1);
            if (eval0)
                free(eval0);
            if (eval1)
                free(eval1);

            int diff = to - from + 1;
            if (diff > 0)
            {
                int r = from + (rand() % diff);
                itoa(r, result, 10);
            }
            else
                itoa(from, result, 10);
        }
        else
            goto error_cleanup;
    }
    else if (strcmp(command, "getkey") == 0)
    {
        keyboard_key_t key = keyboard_input();
        itoa(key.scancode, result, 10);
    }
    else if (strncmp(command, "sleep", 5) == 0)
    {
        args = get_args(command, 6, ' ', 1);
        if (args != NULL)
        {
            char *eval = eval_string(args[0]);
            sleep_ms(atoi(eval));
            if (eval)
                free(eval);
        }
        else
            goto error_cleanup;
    }
    else if (strncmp(command, "eq", 2) == 0)
    {
        args = get_args(command, 3, ' ', 2);
        if (args != NULL)
        {
            char *e1 = eval_string(args[0]);
            char *e2 = eval_string(args[1]);
            if (strcmp(e1, e2) == 0)
                strcpy(result, "1");
            else
                strcpy(result, "0");
            if (e1)
                free(e1);
            if (e2)
                free(e2);
        }
        else
            goto error_cleanup;
    }
    else if (strncmp(command, "neq", 3) == 0)
    {
        args = get_args(command, 4, ' ', 2);
        if (args != NULL)
        {
            char *e1 = eval_string(args[0]);
            char *e2 = eval_string(args[1]);
            if (strcmp(e1, e2) != 0)
                strcpy(result, "1");
            else
                strcpy(result, "0");
            if (e1)
                free(e1);
            if (e2)
                free(e2);
        }
        else
            goto error_cleanup;
    }
    else if (strncmp(command, "gt", 2) == 0)
    {
        args = get_args(command, 3, ' ', 2);
        if (args != NULL)
        {
            char *e1 = eval_string(args[0]);
            char *e2 = eval_string(args[1]);
            if (atoi(e1) > atoi(e2))
                strcpy(result, "1");
            else
                strcpy(result, "0");
            if (e1)
                free(e1);
            if (e2)
                free(e2);
        }
        else
            goto error_cleanup;
    }
    else if (strncmp(command, "lt", 2) == 0)
    {
        args = get_args(command, 3, ' ', 2);
        if (args != NULL)
        {
            char *e1 = eval_string(args[0]);
            char *e2 = eval_string(args[1]);
            if (atoi(e1) < atoi(e2))
                strcpy(result, "1");
            else
                strcpy(result, "0");
            if (e1)
                free(e1);
            if (e2)
                free(e2);
        }
        else
            goto error_cleanup;
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
            printf("\n");
        else
        {
            char *evaluated = eval_string(args[0]);
            printf("%s\n", evaluated);
            if (evaluated)
                free(evaluated);
        }
    }
    else if (strncmp(command, "error", 5) == 0)
    {
        args = get_args(command, 6, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        panic(args[0]);
    }
    else if (strcmp(command, "keys") == 0)
    {
        printf("Press keys to see their scancodes. Press Esc to exit.\n");
        keyboard_key_t key = (keyboard_key_t){0};
        while (key.scancode != Escape)
        {
            key = keyboard_input();
            if (key.scancode != 0)
                printf("Scancode: 0x%x Character: %c\n", key.scancode, key.character);
        }
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
        {
            char *res = evaluate(args[1]);
            if (res)
                free(res);
        }
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
        char *eval_x = eval_string(args[0]);
        int x = atoi(eval_x);
        if (eval_x)
            free(eval_x);
        char *eval_y = eval_string(args[1]);
        int y = atoi(eval_y);
        if (eval_y)
            free(eval_y);
        terminal_setpos(x, y);
    }
    else if (strcmp(command, "getpos") == 0)
    {
        itoa(terminal_getpos(), result, 10);
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
        {
            char *res = evaluate(args[i]);
            if (res)
                free(res);
        }
    }
    else if (strncmp(command, "if", 2) == 0)
    {
        args = get_args(command, 3, ' ', 2);
        if (args == NULL)
            goto error_cleanup;

        char *eval_cond = eval_string(args[0]);
        int cond = atoi(eval_cond);
        if (eval_cond)
            free(eval_cond);

        if (cond != 0)
        {
            char *res = evaluate(args[1]);
            if (res)
                free(res);
        }
    }
    else if (strncmp(command, "while", 5) == 0)
    {
        args = get_args(command, 6, ' ', 2);
        if (args == NULL)
            goto error_cleanup;

        while (true)
        {
            char *eval_cond = eval_string(args[0]);
            int cond = atoi(eval_cond);
            if (eval_cond)
                free(eval_cond);

            if (cond == 0)
                break;

            char *res = evaluate(args[1]);
            if (res)
                free(res);
        }
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
    else if (strcmp(command, "video") == 0)
        printf("Display %dx%d at 0x%lx\nTerminal width: %d height: %d\n",
               fb_get_width(), fb_get_height(), fb_get_address(),
               terminal_width, terminal_height);
    else if (strncmp(command, "sh", 2) == 0)
    {
        args = get_args(command, 3, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        vfs_node_t *file = vfs_find(args[0]);
        if (file)
        {
            char *script_buf = kmalloc(file->length + 1);
            uint32_t real_len = 0;
            for (uint32_t i = 0; i < file->length; i++)
            {
                if (file->data[i] != '\r')
                {
                    script_buf[real_len++] = file->data[i];
                }
            }
            script_buf[real_len] = '\0';

            char *res = evaluate(script_buf);
            if (res)
                free(res);

            free(script_buf);
        }
        else
            printf("File not found: %s\n", args[0]);
    }
    else if (strncmp(command, "bmp", 3) == 0)
    {
        args = get_args(command, 4, ' ', 3);
        if (args == NULL)
            goto error_cleanup;

        char *eval = eval_string(args[0]);
        int x = atoi(eval);
        free(eval);

        eval = eval_string(args[1]);
        int y = atoi(eval);
        free(eval);

        vfs_node_t *file = vfs_find(args[2]);
        if (file)
            draw_bmp_at_position((const uint8_t *)file->data, x, y);
        else
            printf("File not found: %s\n", args[2]);
    }
    else if (strncmp(command, "cat", 3) == 0)
    {
        args = get_args(command, 4, ' ', 1);
        if (args == NULL)
            goto error_cleanup;
        vfs_node_t *file = vfs_find(args[0]);
        if (file)
            printf("%s\n", file->data);
        else
            printf("File not found: %s\n", args[0]);
    }
    else if (strcmp(command, "ls") == 0)
    {
        for (vfs_node_t *file = vfs_root; file != NULL; file = file->next)
        {
            if (file->flags & FS_FILE)
                printf("%s\n", file->name);
        }
    }
    else if (strncmp(command, "mul", 3) == 0)
    {
        args = get_args(command, 4, ' ', 2);
        if (args == NULL)
            goto error_cleanup;

        char *eval = eval_string(args[0]);
        int val0 = atoi(eval);
        free(eval);

        eval = eval_string(args[1]);
        int val1 = atoi(eval);
        free(eval);

        itoa(val0 * val1, result, 10);
    }
    else if (strncmp(command, "go", 2) == 0)
    {
        args = get_args(command, 3, ' ', 1);
        if (args == NULL)
            goto error_cleanup;

        char *res = evaluate(args[0]);
        if (res)
            free(res);
    }
    else if (recursion_depth == 1 && strcmp(command, "exit") != 0 && strcmp(command, "") != 0)
        printf("Unknown command: %s\n", command);
    else if (strcmp(command, "") != 0)
    {
        if (result)
            free(result);
        result = kmalloc(strlen(command) + 1);
        strcpy(result, command);
    }

    if (args)
        free_args(args);
    return result;

error_cleanup:
    free_args(args);
    char *err = kmalloc(6);
    strcpy(err, "ERROR");
    if (result)
        free(result);
    return err;
}