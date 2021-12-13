/* References:
    - https://stackoverflow.com/a/16519793/6562425
    - Students Referenced: Rakhee (Net ID: rr3937)
*/

#include <unistd.h>
#include <stdio.h>
#include "disk.h"
#include "recovery.h"
#include "utils.h"

void list_root(unsigned char *disk, BootEntry *disk_info) {
    int num_entries = 0, bytes_read = sizeof(DirEntry);
    for (unsigned int next_cluster = disk_info->BPB_RootClus; next_cluster < 0x0ffffff8; next_cluster = *read_fat(disk, disk_info, next_cluster, 1)) {
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
    int option, optcount = 0;
    char optflag = ' ';
    char *filename = NULL;  // File to be recovered
    char *sha1 = NULL;  // SHA1 of the file to be recovered

    // Parse options
    while((option = getopt(argc, argv, "ilr:R:s:")) != -1) {
        optcount++;
        switch(option) {
            case 'i':
                optflag = 'i';
                break;
            case 'l':
                optflag = 'l';
                break;
            case 'r':
                filename = optarg;
                optflag = 'r';
                break;
            case 'R':
                filename = optarg;
                optflag = 'R';
                break;
            case 's':
                sha1 = optarg;
                break;
            default:
                optflag = ' ';
                break;
        }
    }

    if (optcount != 1 || optflag == ' ') {
        if (!((optflag == 'r' || optflag =='R') && sha1 != NULL)) {
            display_usage();
            return 0;
        }
    }

    if (optind == argc) {
        display_usage();
        return 0;
    }

    // Get diskname and read the disk
    char *disk_name = argv[optind];
    int disk_size = 1;
    unsigned char *disk = map_disk(disk_name, &disk_size, 'r');
    if (disk == NULL)
        return -1;
    BootEntry *disk_info = (BootEntry *)disk;

    // Recover file
    if (optflag == 'i')
        display_fsinfo(disk_info);
    else if (optflag == 'l')
        list_root(disk, disk_info);
    else if (optflag == 'r')
        recover_contiguous_file(disk, disk_info, disk_name, filename, sha1);
    else if (optflag == 'R') {
        if (sha1 == NULL) {
            display_usage();
            return 0;
        }
        recover_non_contiguous_file(disk, disk_info, disk_name, filename, sha1);
    }

    unmap_disk(disk, disk_size);
    return 0;
}