#include <pci.h>
#include <stdio.h>
#include <io.h>

#define PCI_ADDRESS 0xCF8
#define PCI_DATA 0xCFC
#define PCI_DEV(bus, slot, func) 0x80000000 | (bus << 16) | (slot << 11) | (func << 8)
#define PCI_PORT(port) (port & 0b11)

#define PCI_BUS_MAX 256
#define PCI_SLOT_MAX 32
#define PCI_FUNC_MAX 8

#define PCI_CLASS_ATA 0x01
#define PCI_ATA_SUB_SCSI 0x00  //   SCSI
#define PCI_ATA_SUB_IDE 0x01   //   IDE
#define PCI_ATA_SUB_FDC 0x02   //   FDC
#define PCI_ATA_SUB_IPI 0x03   //   IPI
#define PCI_ATA_SUB_RAID 0x04  //   RAID
#define PCI_ATA_SUB_ATA 0x05   //   ATA controller
#define PCI_ATA_SUB_SATA 0x06  //   Serial ATA
#define PCI_ATA_SUB_SAS 0x07   //   Serial Attached SCSI
#define PCI_ATA_SUB_SSS 0x08   //   Solid State Storage
#define PCI_ATA_SUB_OTHER 0x80 //   Other

const char* pci_ata[] = {
    "SCSI",
    "IDE",
    "FDC",
    "IPI",
    "RAID",
    "ATA controller",
    "Serial ATA",
    "Serial Attached SCSI",
    "Solid State Storage",
    "Other",
};

void pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t *device)
{
    uint32_t address = PCI_DEV(bus, slot, func);

    outl(PCI_ADDRESS, address);

    device->vendor_id = inw(PCI_DATA);
    device->device_id = inw(PCI_DATA + 2);
    device->class_code = inb(PCI_DATA + 9);
    device->subclass = inb(PCI_DATA + 10);
    device->prog_if = inb(PCI_DATA + 11);
    device->header_type = inb(PCI_DATA + 14);
}

uint8_t pci_read_byte(int bus, int slot, int func, int port)
{
    outl(PCI_ADDRESS, PCI_DEV(bus, slot, func) | (port & 0xFC));
    return (inl(PCI_DATA) >> (PCI_PORT(port) * 8));
}

uint16_t pci_read_word(int bus, int slot, int func, int port)
{
    if (PCI_PORT(port) <= 2)
    {
        outl(PCI_ADDRESS, PCI_DEV(bus, slot, func) | (port & 0xFC));
        return (inl(PCI_DATA) >> (PCI_PORT(port) * 8));
    }
    else
    {
        return (pci_read_byte(bus, slot, func, port + 1) << 8) |
               pci_read_byte(bus, slot, func, port);
    }
}

uint32_t pci_read_dword(int bus, int slot, int func, int port)
{
    if (PCI_PORT(port) == 0)
    {
        outl(PCI_ADDRESS, PCI_DEV(bus, slot, func) | (port & 0xFC));
        return (inl(PCI_DATA) >> (PCI_PORT(port) * 8));
    }
    else
    {
        return (pci_read_word(bus, slot, func, port + 2) << 16) |
               pci_read_word(bus, slot, func, port);
    }
}

void show_pci_devices()
{
    pci_device_t pci;

    for (int bus = 0; bus < PCI_BUS_MAX; bus++)
    {
        for (int slot = 0; slot < PCI_SLOT_MAX; slot++)
        {
            for (int func = 0; func < PCI_FUNC_MAX; func++)
            {
                pci_read_config(bus, slot, func, &pci);

                if (pci.vendor_id != 0xFFFF)
                {
                    const char* type = pci_ata[9];
                    if (pci.class_code == PCI_CLASS_ATA && pci.subclass <= PCI_ATA_SUB_SSS)
                        type = pci_ata[pci.subclass];
                    printf("bus 0x%x slot 0x%x func 0x%x ven 0x%x dev 0x%x class 0x%x type %s\n",
                           bus, slot, func, pci.vendor_id, pci.device_id, pci.class_code, type);
                }
            }
        }
    }
}