#pragma once

#include <stdint.h>
#include <stddef.h>

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02

typedef struct vfs_node {
    char name[256];
    uint32_t flags;
    uint32_t length;
    uint8_t *data;
    struct vfs_node *next;
} vfs_node_t;

extern vfs_node_t *vfs_root;

void init_vfs();
void vfs_add_node(vfs_node_t *node);
vfs_node_t *vfs_find(const char *name);
char *vfs_strncmp(const char *name, size_t length);
