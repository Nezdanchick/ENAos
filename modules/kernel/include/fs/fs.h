#pragma once

typedef struct
{
    char name[8];
} filesystem_t;

extern void init_fs();
extern void fs_add(filesystem_t *fs);
