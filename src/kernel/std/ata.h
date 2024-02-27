#include "types.h"
#include "io.h"

void ata_read_sectors(u8* target_address, u32 LBA, u8 sector_count);
void ata_write_sectors(u32 LBA, u8 sector_count, u8 *rawBytes);