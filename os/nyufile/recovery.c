#include <unistd.h>
#include <stdio.h>
#include "recovery.h"
#include "disk.h"
#include "utils.h"
#define print(x) printf("%d\n", x)

void recover_contiguous_file(unsigned char *disk, BootEntry *disk_info, char *disk_name, char *file_name, char *sha1) {
    int num_entries = 0, bytes_read = sizeof(DirEntry), file_found = 0;
    for (unsigned int next_cluster = disk_info->BPB_RootClus; next_cluster < 0x0ffffff8 && !file_found; next_cluster = *read_fat(disk, disk_info, next_cluster, 1)) {
        for (
            DirEntry *entry = read_directory(disk, disk_info, next_cluster);
            entry->DIR_Attr != 0x0 && bytes_read <= disk_info->BPB_BytsPerSec * disk_info->BPB_SecPerClus;
            entry++, bytes_read += sizeof(DirEntry)
        ) {
            if (entry->DIR_Name[0] == 0xe5 && entry->DIR_Attr == 0x20) {
                if (compare_entries(entry->DIR_Name, file_name, 1)) {
                    unsigned int file_cluster = entry->DIR_FstClusHI << 16 | entry->DIR_FstClusLO;
                    unsigned int root_cluster_offset = bytes_read - sizeof(DirEntry);
                    if (write_disk(disk_name, next_cluster, file_cluster, root_cluster_offset, (unsigned char) file_name[0]) == 0) {
                        display_entry_name(entry->DIR_Name);
                        puts(": successfully recovered");
                        file_found = 1;
                        break;
                    }
                }
            }

            num_entries++;
        }
        bytes_read = sizeof(DirEntry);
    }

    if (!file_found)
        printf("%s: file not found\n", file_name);
}