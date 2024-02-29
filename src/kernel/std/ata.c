#include "ata.h"

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40
#define STATUS_DRQ 0x08
#define STATUS_DF 0x20
#define STATUS_ERR 0x01

#define ATA_MASTER_BASE 0x1F0
#define ATA_SLAVE_BASE 0x170

#define ATA_MASTER 0xE0
#define ATA_SLAVE 0xF0

#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

void ATA_wait_BSY() // Wait for bsy to be 0
{
    while (inb(0x1F7) & STATUS_BSY);
}
void ATA_wait_DRQ() // Wait fot drq to be 1
{
    while (!(inb(0x1F7) & STATUS_RDY));
}

bool ata_has_any_disk() {
    return inb(0x1F7) != 0xFF;
}

bool ata_test_rw_bytes() {
    outb(0x1F2, 0x3);
    outb(0x1F3, 0x4);
    return inb(0x1F2) == 0x3 && inb(0x1F3) == 0x4;
}

void ata_read_sectors(u8* target_address, u32 LBA, u8 sector_count) {
    // wait if it's busy
    ATA_wait_BSY();
    // 0xE0 -> master, 0xF0 -> slave, 4 highest bits of LBA
    outb(ATA_MASTER_BASE + ATA_REG_HDDEVSEL,
            ATA_MASTER | ((LBA >> 24) & 0xF));
    // Send the amount of sectors we want
    outb(ATA_MASTER_BASE + ATA_REG_SECCOUNT0, sector_count);
    // Send LBA, 8 bits at a time!
    outb(ATA_MASTER_BASE + ATA_REG_LBA0, (u8)LBA);
    outb(ATA_MASTER_BASE + ATA_REG_LBA1, (u8)(LBA >> 8));
    outb(ATA_MASTER_BASE + ATA_REG_LBA2, (u8)(LBA >> 16));
    // Read already!
    outb(ATA_MASTER_BASE + ATA_REG_COMMAND, 0x20);

    u16* target = (u16*)target_address;

    for (int j = 0; j < sector_count; j++) {
        ATA_wait_BSY();
        ATA_wait_DRQ();
        for (int i = 0; i < 256; i++)
        target[i] = inw(0x1F0);
        target += 256;
    }
}

void ata_write_sectors(u32 LBA, u8 sector_count, u8 *rawBytes) {
  ATA_wait_BSY();
  outb(0x1F6, 0xE0 | ((LBA >> 24) & 0xF));
  outb(0x1F2, sector_count);
  outb(0x1F3, (u8)LBA);
  outb(0x1F4, (u8)(LBA >> 8));
  outb(0x1F5, (u8)(LBA >> 16));
  outb(0x1F7, 0x30); // Send the write command

  u16* bytes = (u16*)rawBytes;

  for (int j = 0; j < sector_count; j++) {
    ATA_wait_BSY();
    ATA_wait_DRQ();
    for (int i = 0; i < 256; i++) {
      outw(0x1F0, bytes[i]);
    }
    bytes += 256;
  }

  // clear cache  
  outb(0x1F7, 0xE7);
  ATA_wait_BSY();
}