#pragma once

#include <stdint.h>

typedef struct
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t header_type;
} pci_device_t;

extern void read_pci_config(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t *device);
extern void show_pci_devices();
