#include <drivers/ata.h>
#include <io.h>
#include <stdio.h>

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40
#define STATUS_DRQ 0x08
#define STATUS_DF 0x20
#define STATUS_ERR 0x01

static void wait_bsy();
static void wait_drq();

void ata_read(uint64_t *target_address, uint32_t lba, uint8_t sector_count)
{
    wait_bsy();

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
    outb(0x1F2, sector_count);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20); // Send the read command

    uint16_t *target = (uint16_t *)target_address;

    for (int j = 0; j < sector_count; j++)
    {
        wait_bsy();
        wait_drq();

        for (int i = 0; i < 256; i++)
            target[i] = inw(0x1F0);

        target += 256;
    }
}

void ata_write(uint32_t *bytes, uint32_t lba, uint8_t sector_count)
{
    wait_bsy();

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
    outb(0x1F2, sector_count);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x30); // Send the write command

    for (int j = 0; j < sector_count; j++)
    {
        wait_bsy();
        wait_drq();

        for (int i = 0; i < 256; i++)
        {
            outl(0x1F0, bytes[i]);
        }
    }
}

static void wait_bsy() // Wait for bsy to be 0
{
    while (inb(0x1F7) & STATUS_BSY)
        ;
}
static void wait_drq() // Wait fot drq to be 1
{
    while (!(inb(0x1F7) & STATUS_RDY))
        ;
}