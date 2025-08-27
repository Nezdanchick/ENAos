#include <stdint.h>

extern void ata_read(uint64_t *target_address, uint32_t lba, uint8_t sector_count);
extern void ata_write(uint32_t *bytes, uint32_t lba, uint8_t sector_count);
