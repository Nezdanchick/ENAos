#include <init.h>
#include <shell.h>

void task_run(void function())
{
    function();
}
void task1()
{
    int i = 0;
    while (true)
    {
        printf("%d", i++ % 10);
        sleep_ms(10);
    }
}
void task2()
{
    char c = 'A';
    while (true)
    {
        printf("%c", c++);
        if (c > 'Z')
            c = 'A';
        sleep_ms(10);
    }
}
void main()
{
    terminal_clear();
    shell("about");

    char *input_buffer = kmalloc(0x100);
    while (true)
    {
        memset(input_buffer, 0, 256);
        printf("/>");
        gets(input_buffer);
        if (shell(input_buffer))
            break;
    }
    printf("Goodbye!\n");
    free(input_buffer);
    outw(0x604, 0x2000); // qemu shutdown
}