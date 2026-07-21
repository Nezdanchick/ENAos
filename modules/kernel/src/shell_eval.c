#include <shell.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <alias.h>
#include <mmu/pmm.h>

int recursion_depth = 0;
int recursion_limit = 32;

void free_args(char **args)
{
    if (!args)
        return;
    for (int i = 0; args[i] != NULL; i++)
    {
        if (args[i])
        {
            free(args[i]);
        }
    }
    free(args);
}

char *eval_string(const char *arg)
{
    if (++recursion_depth > recursion_limit)
    {
        printf("eval recursion limit\n");
        recursion_depth--;
        char *empty = kmalloc(1);
        empty[0] = '\0';
        return empty;
    }
    if (!arg)
    {
        recursion_depth--;
        char *empty = kmalloc(1);
        empty[0] = '\0';
        return empty;
    }

    char *result = kmalloc(512);
    int res_i = 0;
    result[0] = '\0';

    for (int j = 0; arg[j] != '\0'; j++)
    {
        if (arg[j] == '[')
        {
            int depth = 1;
            int start_bracket = j + 1;
            int end_bracket = start_bracket;

            while (arg[end_bracket] != '\0' && depth > 0)
            {
                if (arg[end_bracket] == '[')
                    depth++;
                else if (arg[end_bracket] == ']')
                    depth--;
                if (depth > 0)
                    end_bracket++;
            }

            if (depth == 0)
            {
                int cmd_len = end_bracket - start_bracket;
                char *inner = kmalloc(cmd_len + 1);
                for (int k = 0; k < cmd_len; k++)
                    inner[k] = arg[start_bracket + k];
                inner[cmd_len] = '\0';

                char *inner_eval = eval_string(inner);
                char *eval_res = evaluate(inner_eval);

                free(inner_eval);

                if (eval_res)
                {
                    for (int k = 0; eval_res[k] != '\0' && res_i < 511; k++)
                        result[res_i++] = eval_res[k];
                    free(eval_res);
                }
                free(inner);
                j = end_bracket;
                continue;
            }
        }
        if (res_i < 511)
            result[res_i++] = arg[j];
    }
    result[res_i] = '\0';
    recursion_depth--;
    return result;
}

char **get_args(char *string, size_t start, char separator, int count)
{
    size_t length = strlen(string);
    if (length <= start)
        return NULL;

    char **args = kmalloc(sizeof(char *) * (count + 1));
    char *strcopy = kmalloc(length + 1);
    strcpy(strcopy, string);

    char *curr = &strcopy[start];
    int arg_i = 0, arg_variable = 0;
    char *arg_start = curr;

    for (size_t i = 0; curr[i] != '\0' && arg_i < count; i++)
    {
        if (curr[i] == '[')
            arg_variable++;
        else if (curr[i] == ']')
            arg_variable--;

        if (arg_i == count - 1)
        {
            args[arg_i++] = arg_start;
            for (size_t j = i + 1; curr[j] != '\0'; j++)
            {
                if (curr[j] == '[')
                    arg_variable++;
                else if (curr[j] == ']')
                    arg_variable--;
            }
            break;
        }

        if (arg_variable == 0 && curr[i] == separator)
        {
            curr[i] = '\0';
            args[arg_i++] = arg_start;
            arg_start = &curr[i + 1];
        }
    }

    if (arg_i < count && *arg_start != '\0')
        args[arg_i++] = arg_start;

    while (arg_i <= count)
        args[arg_i++] = NULL;

    if (args[count - 1] == NULL)
    {
        printf("Too few arguments. The command %s needs %d arguments.\n", string, count);
        free(strcopy);
        free(args);
        return NULL;
    }

    for (int i = 0; i < count; i++)
    {
        if (!args[i])
            continue;
        char *cmd = args[i];
        size_t len = strlen(cmd);
        while (len > 0 && cmd[len - 1] == ' ')
            cmd[--len] = '\0';
        while (*cmd == ' ')
        {
            cmd++;
            len--;
        }

        char *copy = kmalloc(strlen(cmd) + 1);
        strcpy(copy, cmd);
        args[i] = copy;
    }
    free(strcopy);
    return args;
}

char **eval_args(char *string, size_t start, char separator, int count)
{
    char **args = get_args(string, start, separator, count);
    if (args == NULL)
        return NULL;

    for (int i = 0; i < count; i++)
    {
        if (args[i] == NULL)
            continue;

        char *evaluated = eval_string(args[i]);
        free(args[i]);
        args[i] = evaluated;
    }
    return args;
}

char *evaluate(char *command)
{
    if (command == NULL)
        return NULL;

    char *alias = get_alias(command);
    if (alias != NULL)
        command = alias;

    char *cmd_copy = kmalloc(strlen(command) + 1);
    strcpy(cmd_copy, command);
    char *orig_copy = cmd_copy;

    if (cmd_copy[0] == '[')
    {
        int last_bracket = -1;
        for (int i = 0; cmd_copy[i] != '\0'; i++)
        {
            if (cmd_copy[i] == ']')
                last_bracket = i;
        }

        bool only_whitespace_after = true;
        for (int i = last_bracket + 1; cmd_copy[i] != '\0'; i++)
        {
            if (cmd_copy[i] != ' ' && cmd_copy[i] != '\n' && cmd_copy[i] != '\t' && cmd_copy[i] != '\r')
            {
                only_whitespace_after = false;
                break;
            }
        }

        if (last_bracket != -1 && only_whitespace_after)
        {
            cmd_copy[last_bracket] = '\0';
            cmd_copy++;
        }
    }

    bool has_newline = false;
    int check_depth = 0;
    for (int i = 0; cmd_copy[i] != '\0'; i++)
    {
        if (cmd_copy[i] == '[')
            check_depth++;
        else if (cmd_copy[i] == ']')
            check_depth--;
        else if (cmd_copy[i] == '\n' && check_depth == 0)
        {
            has_newline = true;
            break;
        }
    }

    if (has_newline)
    {
        int depth = 0;
        int temp_cap = 4096;
        char *temp = kmalloc(temp_cap);
        int temp_i = 0;

        char *last_res = kmalloc(1);
        last_res[0] = '\0';
        bool in_comment = false;

        for (int i = 0;; i++)
        {
            if (cmd_copy[i] == '\0')
            {
                temp[temp_i] = '\0';
                if (temp_i > 0)
                {
                    char *res = evaluate(temp);
                    if (res)
                    {
                        free(last_res);
                        last_res = res;
                    }
                }
                break;
            }

            if (in_comment)
            {
                if (cmd_copy[i] == '\n')
                {
                    in_comment = false;
                    if (depth == 0)
                    {
                        temp[temp_i] = '\0';
                        if (temp_i > 0)
                        {
                            char *res = evaluate(temp);
                            if (res)
                            {
                                free(last_res);
                                last_res = res;
                            }
                        }
                        temp_i = 0;
                    }
                }
                continue;
            }

            if (cmd_copy[i] == '/' && cmd_copy[i + 1] == '/')
            {
                in_comment = true;
                i++;
                continue;
            }

            if (cmd_copy[i] == '[')
                depth++;
            else if (cmd_copy[i] == ']')
                depth--;

            if (cmd_copy[i] == '\n' && depth == 0)
            {
                temp[temp_i] = '\0';
                if (temp_i > 0)
                {
                    char *res = evaluate(temp);
                    if (res)
                    {
                        free(last_res);
                        last_res = res;
                    }
                }
                temp_i = 0;
            }
            else
            {
                if (temp_i >= temp_cap - 1)
                {
                    temp_cap *= 2;
                    char *new_temp = kmalloc(temp_cap);
                    for (int k = 0; k < temp_i; k++)
                        new_temp[k] = temp[k];
                    free(temp);
                    temp = new_temp;
                }
                temp[temp_i++] = cmd_copy[i];
            }
        }
        free(temp);
        free(orig_copy);
        return last_res;
    }

    for (int i = 0; cmd_copy[i] != '\0'; i++)
    {
        if (cmd_copy[i] == '/' && cmd_copy[i + 1] == '/')
        {
            cmd_copy[i] = '\0';
            break;
        }
    }

    int clen = strlen(cmd_copy);
    while (clen > 0 && (cmd_copy[clen - 1] == ' ' || cmd_copy[clen - 1] == '\t' || cmd_copy[clen - 1] == '\r'))
    {
        cmd_copy[--clen] = '\0';
    }

    char *real_cmd = cmd_copy;
    while (*real_cmd == ' ' || *real_cmd == '\t')
    {
        real_cmd++;
    }

    if (strlen(real_cmd) == 0)
    {
        char *empty = kmalloc(1);
        empty[0] = '\0';
        free(orig_copy);
        return empty;
    }

    if (++recursion_depth > recursion_limit)
    {
        recursion_depth--;
        char *empty = kmalloc(1);
        empty[0] = '\0';
        free(orig_copy);
        return empty;
    }

    char *result = execute_builtin(real_cmd);

    recursion_depth--;
    free(orig_copy);
    return result;
}