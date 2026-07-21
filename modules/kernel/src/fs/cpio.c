#include <fs/vfs.h>
#include <string.h>
#include <memory.h>
#include <mmu/pmm.h>
#include <stdio.h>

struct cpio_newc_header {
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};

static uint32_t parse_hex8(const char *str) {
    uint32_t val = 0;
    for (int i = 0; i < 8; i++) {
        val <<= 4;
        if (str[i] >= '0' && str[i] <= '9') val |= (str[i] - '0');
        else if (str[i] >= 'a' && str[i] <= 'f') val |= (str[i] - 'a' + 10);
        else if (str[i] >= 'A' && str[i] <= 'F') val |= (str[i] - 'A' + 10);
    }
    return val;
}

#define ALIGN4(val) (((val) + 3) & ~3)

void init_cpio(uint8_t *archive) {
    uint8_t *ptr = archive;
    
    while (1) {
        struct cpio_newc_header *header = (struct cpio_newc_header *)ptr;
        
        if (strncmp(header->c_magic, "070701", 6) != 0) {
            printf("Invalid CPIO magic: %.6s\n", header->c_magic);
            break;
        }
        
        uint32_t filesize = parse_hex8(header->c_filesize);
        uint32_t namesize = parse_hex8(header->c_namesize);
        uint32_t mode = parse_hex8(header->c_mode);
        
        ptr += sizeof(struct cpio_newc_header);
        
        char *name = (char *)ptr;
        
        if (strcmp(name, "TRAILER!!!") == 0)
            break;
        
        ptr += namesize;
        
        uint32_t name_pad = ALIGN4(sizeof(struct cpio_newc_header) + namesize) - (sizeof(struct cpio_newc_header) + namesize);
        ptr += name_pad;
        
        uint8_t *data = ptr;
        
        vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
        memset(node, 0, sizeof(vfs_node_t));
        
        uint32_t i;
        for (i = 0; i < 255 && i < namesize - 1; i++) {
            node->name[i] = name[i];
        }
        node->name[i] = '\0';
        
        node->length = filesize;
        node->data = data;
        if ((mode & 0xF000) == 0x4000) {
            node->flags = FS_DIRECTORY;
        } else {
            node->flags = FS_FILE;
        }
        
        vfs_add_node(node);
        printf("CPIO loaded: %s (%d bytes)\n", node->name, filesize);
        
        ptr += filesize;
        ptr += ALIGN4(filesize) - filesize;
    }
}
