#include <pci.h>
#include <stdio.h>
#include <asm.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

void read_pci_config(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t *device)
{
    uint32_t address = 0x80000000 | (bus << 16) | (slot << 11) | (func << 8);

    outl(PCI_CONFIG_ADDRESS, address);

    device->vendor_id = inw(PCI_CONFIG_DATA);
    device->device_id = inw(PCI_CONFIG_DATA + 2);
    device->class_code = inb(PCI_CONFIG_DATA + 9);
    device->subclass = inb(PCI_CONFIG_DATA + 10);
    device->prog_if = inb(PCI_CONFIG_DATA + 11);
    device->header_type = inb(PCI_CONFIG_DATA + 14);
}

void show_pci_devices()
{
    pci_device_t pci;

    for (int bus = 0; bus < 256; bus++)
    {
        for (int slot = 0; slot < 32; slot++)
        {
            for (int func = 0; func < 8; func++)
            {
                read_pci_config(bus, slot, func, &pci);

                if (pci.vendor_id != 0xFFFF)
                {
                    printf("bus 0x%x slot 0x%x func 0x%x ven 0x%x dev 0x%x class 0x%x\n",
                        bus, slot, func, pci.vendor_id, pci.device_id, pci.class_code);
                }
            }
        }
    }
}