#include "types.h"
#include "io.h"

bool ata_has_any_disk();
bool ata_test_rw_bytes();
void ata_read_sectors(u8* target_address, u32 LBA, u8 sector_count);
void ata_write_sectors(u32 LBA, u8 sector_count, u8 *rawBytes);