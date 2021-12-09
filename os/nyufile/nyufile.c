#include <unistd.h>
#include <stdio.h>
#include "disk.h"
#include "utils.h"
#define print(x) printf("%d\n", x)

unsigned int ROOT_SECTOR = -1;
unsigned int FAT1_SECTOR = -1;

void display_usage() {
    puts("Usage: ./nyufile disk <options>");
    puts("  -i                     Print the file system information.");
    puts("  -l                     List the root directory.");
    puts("  -r filename [-s sha1]  Recover a contiguous file.");
    puts("  -R filename -s sha1    Recover a possibly non-contiguous file.");
}

void list_root(BootEntry *disk, int disk_fd) {
    DirEntry *root = read_directory(disk_fd, ROOT_SECTOR * disk->BPB_BytsPerSec);
    if (root == NULL)
        return;
    
    int num_entries = 0;
    for (DirEntry *entry = root; entry->DIR_Attr != 0x0; entry++) {
        if (entry->DIR_Name[0] == 0xe5)
            continue;
        
        display_entry_name(entry->DIR_Name);
        
        if (entry->DIR_Attr == 0x10)
            printf("/");
        
        printf(" (size = %u, starting cluster = %d)\n", entry->DIR_FileSize, entry->DIR_FstClusLO);
        num_entries++;
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
    int disk_fd = -1;
    BootEntry *disk = read_disk(disk_name, &disk_fd);
    if (disk == NULL)
        return -1;
    
    // Get the starting root and fat1 sector
    ROOT_SECTOR = disk->BPB_RsvdSecCnt + disk->BPB_NumFATs * disk->BPB_FATSz32 + (disk->BPB_RootClus - 2) * disk->BPB_SecPerClus;
    FAT1_SECTOR = disk->BPB_RsvdSecCnt;

    // Parse options
    while((option = getopt(argc, argv, "ilr:R:s:")) != -1) {
        no_option = 0;
        switch(option) {
            case 'i':
                display_fsinfo(disk);
                break;
            case 'l':
                list_root(disk, disk_fd);
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
    
    close(disk_fd);
    return 0;
}