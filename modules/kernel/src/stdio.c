#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <keyboard.h>
#include <timer.h>
#include <serial.h>
#include <history.h>
#include <gcursor.h>
#include <fb_terminal.h>

int terminal_x = 0;
int terminal_y = 0;
int terminal_width = 80;
int terminal_height = 25;

void stdio_stub() {}
void stdio_stub_write(__attribute__((unused)) char *string) {}
void stdio_stub_setpos(
    __attribute__((unused)) int x,
    __attribute__((unused)) int y) {}

backspace *terminal_backspace = stdio_stub;
clear *terminal_clear = stdio_stub;
scroll *terminal_scroll = stdio_stub;
stdout *terminal_output = stdio_stub_write;
setpos *cursor_setpos = stdio_stub_setpos;

// set std functions
void set_backspace(backspace *func)
{
    terminal_backspace = func;
}
void set_stdout(stdout *func)
{
    terminal_output = func;
}
void set_cursor_setpos(setpos *func)
{
    cursor_setpos = func;
}
void set_scroll(scroll *func)
{
    terminal_scroll = func;
}
void set_clear(clear *func)
{
    terminal_clear = func;
}

// terminal positioning functions
int terminal_getpos()
{
    terminal_check_position();
    return terminal_x + terminal_y * terminal_width;
}
void terminal_check_position()
{
    if (terminal_x >= terminal_width)
    {
        terminal_y += terminal_x / terminal_width;
        terminal_x = terminal_x % terminal_width;
    }
    if (terminal_y >= terminal_height - 1)
        terminal_scroll();
}
void terminal_setpos(int collumn, int row)
{
    terminal_x = collumn;
    terminal_y = row;
    terminal_check_position();
}

// terminal input
char *gets(char *string)
{
    char *address = string;

    keyboard_key_t key = (keyboard_key_t){0};
    while (key.scancode != Enter)
    {
        key = keyboard_input();
        switch (key.scancode)
        {
        case 0:
            break;
        case Backspace:
            if (address > string)
            {
                address--;
                *address = '\0';
            }
            break;
        default:
            if (is_key_printable(key))
            {
                *address++ = key.character;
                *address = '\0';
            }
            break;
        }
    }

    *address = '\0';
    return string;
}

static void clear_text(int x, int y, int len)
{
    terminal_setpos(x + len + 1, y);
    terminal_check_position();
    for (int i = -1; i <= len; i++)
        terminal_backspace();
    terminal_setpos(x, y);
}

static void draw_buffer(int x, int y, char *buffer)
{
    terminal_setpos(x, y);
    fb_write(buffer);
}

char *terminal_gets(char *string)
{
    static char temp[256] = {0};
    static bool is_temp = false;
    char buffer[256] = {0};
    int buf_len = 0;
    int cursor_pos = 0;
    int start_x = terminal_x;
    int start_y = terminal_y;

    gcursor_show();

    keyboard_key_t key = (keyboard_key_t){0};
    while (key.scancode != Enter)
    {
        gcursor_set(start_x + cursor_pos, start_y);
        gcursor_update();

        key = keyboard_input();
        
        if (key.scancode == 0) {
            if (serial_received()) {
                char c = serial_read();
                if (c == '\r' || c == '\n') {
                    key.scancode = Enter;
                    key.character = '\n';
                } else if (c == '\b' || c == 127) {
                    key.scancode = Backspace;
                } else {
                    key.scancode = char_to_scancode(c);
                    key.character = c;
                }
            } else {
                continue;
            }
        }

        switch (key.scancode)
        {
        case LeftArrow:
            if (cursor_pos > 0)
                cursor_pos--;
            break;

        case RightArrow:
            if (cursor_pos < buf_len)
                cursor_pos++;
            break;

        case Home:
            cursor_pos = 0;
            break;

        case End:
            cursor_pos = buf_len;
            break;

        case UpArrow:
        {
            const char *prev = history_get_prev();
            if (prev)
            {
                if (!is_temp && buf_len > 0)
                {
                    strcpy(temp, buffer);
                    is_temp = true;
                }
                clear_text(start_x, start_y, buf_len);
                strcpy(buffer, prev);
                buf_len = strlen(buffer);
                cursor_pos = buf_len;
                draw_buffer(start_x, start_y, buffer);
            }
        }
        break;

        case DownArrow:
        {
            const char *next = history_get_next();
            clear_text(start_x, start_y, buf_len);

            if (next)
            {
                strcpy(buffer, next);
            }
            else
            {
                strcpy(buffer, temp);
                is_temp = false;
            }
            buf_len = strlen(buffer);
            cursor_pos = buf_len;
            draw_buffer(start_x, start_y, buffer);
        }
        break;

        case Backspace:
            if (cursor_pos > 0)
            {
                for (int i = cursor_pos - 1; i < buf_len - 1; i++)
                    buffer[i] = buffer[i + 1];
                buf_len--;
                cursor_pos--;
                buffer[buf_len] = '\0';

                clear_text(start_x, start_y, buf_len);
                draw_buffer(start_x, start_y, buffer);
            }
            break;

        case Delete:
            if (cursor_pos < buf_len)
            {
                for (int i = cursor_pos; i < buf_len - 1; i++)
                    buffer[i] = buffer[i + 1];
                buf_len--;
                buffer[buf_len] = '\0';

                clear_text(start_x, start_y, buf_len);
                draw_buffer(start_x, start_y, buffer);
            }
            break;

        case Enter:
            gcursor_hide();
            gcursor_update();
            break;

        default:
            if (is_key_printable(key) && buf_len < 255)
            {
                if (buf_len == 255)
                    break;

                for (int i = buf_len; i > cursor_pos; i--)
                    buffer[i] = buffer[i - 1];
                buffer[cursor_pos] = key.character;
                buf_len++;
                cursor_pos++;
                buffer[buf_len] = '\0';

                clear_text(start_x, start_y, buf_len);
                draw_buffer(start_x, start_y, buffer);
            }
            else if (key.alt)
            {
            }

            break;
        }
    }

    gcursor_hide();

    strcpy(string, buffer);

    terminal_x = 0;
    terminal_y++;
    serial_write(string);
    putserial('\n');
    terminal_check_position();

    if (buf_len > 0)
        history_add(string);

    history_reset_position();

    return string;
}
