#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"

void display_fsinfo(BootEntry *disk) {
    printf("Number of FATs = %d\n", disk->BPB_NumFATs);
    printf("Number of bytes per sector = %d\n", disk->BPB_BytsPerSec);
    printf("Number of sectors per cluster = %d\n", disk->BPB_SecPerClus);
    printf("Number of reserved sectors = %d\n", disk->BPB_RsvdSecCnt);
}

DirEntry *read_directory(int disk_fd, unsigned int offset) {
    DirEntry *dir = mmap(NULL, sizeof(DirEntry), PROT_READ, MAP_PRIVATE, disk_fd, offset);
    if (dir == MAP_FAILED) {
        fprintf(stderr, "Error reading directory\n");
        return NULL;
    }
    return dir;
}

BootEntry *read_disk(char *disk_name, int *disk_fd) {
    *disk_fd = open(disk_name, O_RDONLY);
    if (*disk_fd < 0) {
        fprintf(stderr, "Error opening disk\n");
        return NULL;
    }

    BootEntry *disk = mmap(NULL, sizeof(BootEntry), PROT_READ, MAP_SHARED, *disk_fd, 0);
    if (disk == MAP_FAILED) {
        fprintf(stderr, "Error reading disk\n");
        return NULL;
    }

    return disk;
}
