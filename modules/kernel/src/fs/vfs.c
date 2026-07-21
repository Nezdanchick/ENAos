#include <fs/vfs.h>
#include <string.h>
#include <memory.h>

vfs_node_t *vfs_root = NULL;

void init_vfs()
{
    vfs_root = NULL;
}

void vfs_add_node(vfs_node_t *node)
{
    if (!vfs_root)
    {
        vfs_root = node;
    }
    else
    {
        vfs_node_t *curr = vfs_root;
        while (curr->next)
        {
            curr = curr->next;
        }
        curr->next = node;
    }
}

vfs_node_t *vfs_find(const char *name)
{
    vfs_node_t *curr = vfs_root;
    if (name[0] == '/')
        name++;
    if (name[0] == '.' && name[1] == '/')
        name += 2;

    while (curr)
    {
        const char *curr_name = curr->name;
        if (curr_name[0] == '.' && curr_name[1] == '/')
            curr_name += 2;

        if (strcmp(curr_name, name) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

char *vfs_strncmp(const char *name, size_t length)
{
    vfs_node_t *curr = vfs_root;
    if (name[0] == '/')
        name++;
    if (name[0] == '.' && name[1] == '/')
        name += 2;

    while (curr)
    {
        const char *curr_name = curr->name;
        if (curr_name[0] == '.' && curr_name[1] == '/')
            curr_name += 2;

        if (strncmp(curr_name, name, length) == 0)
        {
            return curr->name;
        }
        curr = curr->next;
    }
    return NULL;
}
