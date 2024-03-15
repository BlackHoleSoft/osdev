#include "fs.h"
#include "ata.h"
#include "mem.h"
#include "string.h"

struct FSTableItem* fs_get_file(int sector) {
    struct FSTableItem* file = mem_512();
    ata_read_sectors((u8*)file, sector, 1);
    return file;
}

char* fs_get_file_contents(struct FSTableItem* file) {
    char* buffer = mem_10kb();
    // read no more than 10 kb
    for (int i=0; i<file->size && i<20; i++) {
        ata_read_sectors(buffer + i * 512, file->sectors[i], 1);        
    }
    return buffer;
}

void fs_add_file(struct FSTableItem* file, char* contents, int content_length) {
    char* tmp_data = mem_512();
    int header_sector = 0;
    int max_sector = 0;
    for (int i=FS_TABLE_START_SECTOR; i<FS_TABLE_SIZE; i++) {
        ata_read_sectors(tmp_data, i, 1);
        if (tmp_data[0] == 0 && header_sector == 0) {
            header_sector = i;            
        } else {
            struct FSTableItem* fsfile = (struct FSTableItem*)tmp_data;
            for (int j=0; fsfile->sectors[j] > 0; j++) {
                if (fsfile->sectors[j] > max_sector) {
                    max_sector = fsfile->sectors[j];
                }                
            }
        }
    }
    mem_free(tmp_data);

    // write data
    int length_sec = content_length / 512 + 1;
    ata_write_sectors(max_sector + 1, length_sec, contents);
    for (int i=0; i<length_sec; i++) {
        file->sectors[i] = max_sector + 1 + i;
    }
    file->size = length_sec;

    str_copy(file->create_date, "2222-01-01T00:00:00");
    str_copy(file->update_date, "2222-01-01T00:00:00");

    // write header
    ata_write_sectors(header_sector, 1, (u8*)file);
}