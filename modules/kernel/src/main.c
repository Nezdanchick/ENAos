#include <init.h>
#include <shell.h>
#include <alias.h>
#include <history.h>
#include <gcursor.h>

void kmain()
{
    init_alias();
    init_history();

    char *w_str = kmalloc(0x20);
    set_alias("FBWIDTH", itoa(fb_get_width(), w_str, 10));
    free(w_str);
    
    char *h_str = kmalloc(0x20);
    set_alias("FBHEIGHT", itoa(fb_get_height(), h_str, 10));
    free(h_str);

    char *res1 = evaluate("sh /sys/init.ena");
    free(res1);

    timer_add_callback(gcursor_blink, 1000);

    shell();
    
    printf("Goodbye!\n");
    outw(0x604, 0x2000); // qemu shutdown
}