#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <pci.h>
#include <io.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#define PCI_ENABLE 0x80000000

#define PCI_BUS_MAX 256
#define PCI_SLOT_MAX 32
#define PCI_FUNC_MAX 8

static uint32_t pci_config_address(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    return (uint32_t)(PCI_ENABLE |
                      ((uint32_t)bus << 16) |
                      ((uint32_t)slot << 11) |
                      ((uint32_t)func << 8) |
                      (offset & 0xFC));
}

uint32_t pci_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    outl(PCI_CONFIG_ADDRESS, pci_config_address(bus, slot, func, offset));
    return inl(PCI_CONFIG_DATA);
}

uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t value = pci_read_dword(bus, slot, func, offset);
    return (uint16_t)((value >> ((offset & 2) * 8)) & 0xFFFF);
}

uint8_t pci_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t value = pci_read_dword(bus, slot, func, offset);
    return (uint8_t)((value >> ((offset & 3) * 8)) & 0xFF);
}

void pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t *device)
{
    device->vendor_id = pci_read_word(bus, slot, func, 0x00);
    device->device_id = pci_read_word(bus, slot, func, 0x02);
    device->prog_if = pci_read_byte(bus, slot, func, 0x09);
    device->subclass = pci_read_byte(bus, slot, func, 0x0A);
    device->class_code = pci_read_byte(bus, slot, func, 0x0B);
    device->header_type = pci_read_byte(bus, slot, func, 0x0E);
}

static const char *pci_class_name(uint8_t class_code)
{
    switch (class_code)
    {
    case 0x00:
        return "Unclassified";
    case 0x01:
        return "Mass storage controller";
    case 0x02:
        return "Network controller";
    case 0x03:
        return "VGA compatible controller";
    case 0x04:
        return "Multimedia controller";
    case 0x05:
        return "Memory controller";
    case 0x06:
        return "Bridge";
    case 0x07:
        return "Communication controller";
    case 0x08:
        return "System peripheral";
    case 0x09:
        return "Input device controller";
    case 0x0A:
        return "Docking station";
    case 0x0B:
        return "Processor";
    case 0x0C:
        return "Serial bus controller";
    default:
        return "Unknown device";
    }
}

static void pci_scan_bus(uint8_t bus);

static void pci_scan_device(uint8_t bus, uint8_t slot)
{
    uint16_t vendor = pci_read_word(bus, slot, 0, 0x00);
    if (vendor == 0xFFFF)
        return;

    uint8_t header = pci_read_byte(bus, slot, 0, 0x0E);
    uint8_t functions = (header & 0x80) ? PCI_FUNC_MAX : 1;

    for (uint8_t func = 0; func < functions; func++)
    {
        pci_device_t dev;
        pci_read_config(bus, slot, func, &dev);
        if (dev.vendor_id == 0xFFFF)
            continue;

        printf("%02x:%02x.%x %s: [%04x:%04x]\n",
               bus,
               slot,
               func,
               pci_class_name(dev.class_code),
               dev.vendor_id,
               dev.device_id);

        if (dev.class_code == 0x06 && dev.subclass == 0x04)
        {
            uint8_t secondary_bus = pci_read_byte(bus, slot, func, 0x19);
            pci_scan_bus(secondary_bus);
        }
    }
}

static void pci_scan_bus(uint8_t bus)
{
    for (uint8_t slot = 0; slot < PCI_SLOT_MAX; slot++)
        pci_scan_device(bus, slot);
}

void show_pci_devices()
{
    uint8_t header = pci_read_byte(0, 0, 0, 0x0E);
    if ((header & 0x80) == 0)
    {
        pci_scan_bus(0);
    }
    else
    {
        for (uint8_t func = 0; func < PCI_FUNC_MAX; func++)
        {
            if (pci_read_word(0, 0, func, 0x00) != 0xFFFF)
            {
                uint8_t bus = pci_read_byte(0, 0, func, 0x19);
                pci_scan_bus(bus);
            }
        }
    }
}

static const char *usb_prog_if_name(uint8_t prog_if)
{
    switch (prog_if)
    {
    case 0x00:
        return "UHCI";
    case 0x10:
        return "OHCI";
    case 0x20:
        return "EHCI";
    case 0x30:
        return "XHCI";
    case 0x80:
        return "Unspecified";
    case 0xFE:
        return "USB Device";
    default:
        return "Unknown USB";
    }
}

static const char *get_vendor_name(uint16_t vendor_id)
{
    switch (vendor_id)
    {
    case 0x1d6b:
        return "Linux Foundation";
    case 0x8086:
        return "Intel Corp.";
    case 0x13d3:
        return "IMC Networks";
    case 0x06cb:
        return "Synaptics, Inc.";
    case 0x0bda:
        return "Realtek Semiconductor Corp.";
    default:
        return NULL;
    }
}

static void print_usb_device(uint8_t bus_num, uint8_t device_num, uint16_t vendor_id, uint16_t device_id, const char *description)
{
    const char *vendor_name = get_vendor_name(vendor_id);

    if (vendor_name)
    {
        printf("Bus %03d Device %03d: ID %04x:%04x %s %s\n",
               bus_num, device_num, vendor_id, device_id, vendor_name, description);
    }
    else
    {
        printf("Bus %03d Device %03d: ID %04x:%04x %s\n",
               bus_num, device_num, vendor_id, device_id, description);
    }
}

void show_usb_devices()
{
    uint8_t usb_bus_counter = 1;
    uint8_t usb_device_counter = 1;

    for (uint16_t bus = 0; bus < PCI_BUS_MAX; bus++)
    {
        uint8_t found_on_bus = 0;

        for (uint8_t slot = 0; slot < PCI_SLOT_MAX; slot++)
        {
            uint16_t vendor = pci_read_word(bus, slot, 0, 0x00);
            if (vendor == 0xFFFF)
                continue;

            uint8_t header = pci_read_byte(bus, slot, 0, 0x0E);
            uint8_t functions = (header & 0x80) ? PCI_FUNC_MAX : 1;

            for (uint8_t func = 0; func < functions; func++)
            {
                pci_device_t dev;
                pci_read_config(bus, slot, func, &dev);
                if (dev.vendor_id == 0xFFFF)
                    continue;

                if (dev.class_code == 0x0C && dev.subclass == 0x03)
                {
                    if (!found_on_bus)
                    {
                        found_on_bus = 1;
                        usb_device_counter = 1;
                    }

                    const char *usb_type = usb_prog_if_name(dev.prog_if);

                    if (dev.prog_if == 0x00 || dev.prog_if == 0x10 || dev.prog_if == 0x20 || dev.prog_if == 0x30)
                    {
                        print_usb_device(usb_bus_counter, usb_device_counter++,
                                         0x1d6b, 0x0002 + (dev.prog_if == 0x30 ? 1 : 0),
                                         usb_type);
                    }
                    else
                    {
                        print_usb_device(usb_bus_counter, usb_device_counter++,
                                         dev.vendor_id, dev.device_id,
                                         usb_type);
                    }
                }

                if (dev.class_code == 0x06 && dev.subclass == 0x04)
                {
                    uint8_t secondary_bus = pci_read_byte(bus, slot, func, 0x19);
                    for (uint8_t s = 0; s < PCI_SLOT_MAX; s++)
                    {
                        uint16_t v = pci_read_word(secondary_bus, s, 0, 0x00);
                        if (v == 0xFFFF)
                            continue;

                        uint8_t h = pci_read_byte(secondary_bus, s, 0, 0x0E);
                        uint8_t fcount = (h & 0x80) ? PCI_FUNC_MAX : 1;

                        for (uint8_t f = 0; f < fcount; f++)
                        {
                            pci_device_t subdev;
                            pci_read_config(secondary_bus, s, f, &subdev);
                            if (subdev.vendor_id == 0xFFFF)
                                continue;

                            if (subdev.class_code == 0x0C && subdev.subclass == 0x03)
                            {
                                if (!found_on_bus)
                                {
                                    found_on_bus = 1;
                                    usb_device_counter = 1;
                                }

                                const char *usb_type = usb_prog_if_name(subdev.prog_if);
                                print_usb_device(usb_bus_counter, usb_device_counter++,
                                                 subdev.vendor_id, subdev.device_id,
                                                 usb_type);
                            }
                        }
                    }
                }
            }
        }

        if (found_on_bus)
        {
            usb_bus_counter++;
        }
    }
}