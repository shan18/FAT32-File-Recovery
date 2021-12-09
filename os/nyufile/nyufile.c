#include <unistd.h>
#include <stdio.h>
#include "disk.h"
#include "utils.h"
#define print(x) printf("%d\n", x)

void list_root(unsigned char *disk, BootEntry *disk_info) {
    int num_entries = 0, bytes_read = sizeof(DirEntry);
    for (unsigned int next_cluster = disk_info->BPB_RootClus; next_cluster < 0x0ffffff8; next_cluster = *read_fat(disk, disk_info, next_cluster)) {
        for (
            DirEntry *entry = read_directory(disk, disk_info, next_cluster);
            entry->DIR_Attr != 0x0 && bytes_read <= disk_info->BPB_BytsPerSec * disk_info->BPB_SecPerClus;
            entry++, bytes_read += sizeof(DirEntry)
        ) {
            if (entry->DIR_Name[0] == 0xe5)
                continue;
            
            display_entry_name(entry->DIR_Name);
            
            if (entry->DIR_Attr == 0x10)
                printf("/");
            
            unsigned int start_cluster = entry->DIR_FstClusHI << 16 | entry->DIR_FstClusLO;
            printf(" (size = %u, starting cluster = %u)\n", entry->DIR_FileSize, start_cluster);
            num_entries++;
        }
        bytes_read = sizeof(DirEntry);
    }

    printf("Total number of entries = %d\n", num_entries);
}

int main(int argc, char *argv[]) {
    int option, no_option = 1;
    int recovery_mode = -1;  // -1: no recovery, 0: recover contiguous, 1: recover non-contiguous
    char *filename = NULL;  // File to be recovered
    char *sha1 = NULL;  // SHA1 of the file to be recovered

    if (optind == argc) {
        display_usage();
        return 0;
    }

    // Get diskname and read the disk
    char *disk_name = argv[optind];
    unsigned char *disk = read_disk(disk_name);
    if (disk == NULL)
        return -1;
    BootEntry *disk_info = (BootEntry *)disk;

    // Parse options
    while((option = getopt(argc, argv, "ilr:R:s:")) != -1) {
        no_option = 0;
        switch(option) {
            case 'i':
                display_fsinfo(disk_info);
                break;
            case 'l':
                list_root(disk, disk_info);
                break;
            case 'r':
                filename = optarg;
                recovery_mode = 0;
                break;
            case 'R':
                filename = optarg;
                recovery_mode = 1;
                break;
            case 's':
                sha1 = optarg;
                break;
            default:
                display_usage();
                break;
        }
    }

    // Recover file
    if (recovery_mode == 0) {
        puts("Recovering contiguous file");
    } else if (recovery_mode == 1) {
        if (sha1 == NULL) {
            display_usage();
            return 0;
        }
        puts("Recovering non-contiguous file");
    }

    // If no option given, show usage and exit
    if(no_option)
        display_usage();
    
    return 0;
}