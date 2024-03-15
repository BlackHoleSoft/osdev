#include "types.h"

#define FS_TABLE_SIZE 0xFF
#define FS_TABLE_START_SECTOR 0x1

struct FSHeader {
    char fsname[12];
    u16 last_file_sector;
};

struct FSTableItem {
    char name[64];
    char create_date[20];
    char update_date[20];
    u16 size;
    char reserved[128 - 40 - 2];
    u16 sectors[(512 - 64 - 128) / 2];
};

struct FSTableItem* fs_get_file(int sector);
char* fs_get_file_contents(struct FSTableItem* file);
void fs_add_file(struct FSTableItem* file, char* contents, int content_length);