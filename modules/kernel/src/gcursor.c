#include <gcursor.h>
#include <fb_terminal.h>

static int cursor_visible = 0;
static int cursor_x = 0;
static int cursor_y = 0;

void gcursor_show()
{
    cursor_visible = 1;
}

void gcursor_hide()
{
    cursor_visible = 0;
}

void gcursor_set(int x, int y)
{
    cursor_x = x;
    cursor_y = y;
}

void gcursor_move(int dx, int dy)
{
    cursor_x += dx;
    cursor_y += dy;
}

int gcursor_get_x()
{
    return cursor_x;
}

int gcursor_get_y()
{
    return cursor_y;
}
void gcursor_update()
{
    static int last_cursor_x = -1;
    static int last_cursor_y = -1;
    
    if (!cursor_visible)
    {
        if (last_cursor_x != -1)
        {
            fb_draw_cursor(last_cursor_x, last_cursor_y, fb_bg_color);
            last_cursor_x = -1;
            last_cursor_y = -1;
        }
        return;
    }
    
    int cur_x = gcursor_get_x();
    int cur_y = gcursor_get_y();
    
    if (cur_x != last_cursor_x || cur_y != last_cursor_y)
    {
        if (last_cursor_x != -1)
        {
            fb_draw_cursor(last_cursor_x, last_cursor_y, fb_bg_color);
        }
        
        fb_draw_cursor(cur_x, cur_y, fb_fg_color);
        last_cursor_x = cur_x;
        last_cursor_y = cur_y;
    }
}