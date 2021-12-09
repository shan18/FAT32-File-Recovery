#include <unistd.h>
#include <stdio.h>
#include "disk.h"
#define print(x) printf("%d\n", x)

void display_usage() {
    puts("Usage: ./nyufile disk <options>");
    puts("  -i                     Print the file system information.");
    puts("  -l                     List the root directory.");
    puts("  -r filename [-s sha1]  Recover a contiguous file.");
    puts("  -R filename -s sha1    Recover a possibly non-contiguous file.");
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
    BootEntry *disk = read_disk(disk_name);
    if (disk == NULL)
        return -1;

    // Parse options
    while((option = getopt(argc, argv, "ilr:R:s:")) != -1) {
        no_option = 0;
        switch(option) {
            case 'i':
                display_fsinfo(disk);
                break;
            case 'l':
                puts("-l");
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