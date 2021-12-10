#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"
#include "utils.h"
#define print(x) printf("%d\n", x)

void display_fsinfo(BootEntry *disk_info) {
    printf("Number of FATs = %d\n", disk_info->BPB_NumFATs);
    printf("Number of bytes per sector = %d\n", disk_info->BPB_BytsPerSec);
    printf("Number of sectors per cluster = %d\n", disk_info->BPB_SecPerClus);
    printf("Number of reserved sectors = %d\n", disk_info->BPB_RsvdSecCnt);
}

unsigned int cluster_to_sector(BootEntry *disk_info, unsigned int cluster) {
    return disk_info->BPB_RsvdSecCnt + disk_info->BPB_NumFATs * disk_info->BPB_FATSz32 + (cluster - 2) * disk_info->BPB_SecPerClus;
}

unsigned int cluster_to_bytes(BootEntry *disk_info, unsigned int cluster) {
    return cluster_to_sector(disk_info, cluster) * disk_info->BPB_BytsPerSec;
}

unsigned int *read_fat(unsigned char *disk, BootEntry *disk_info, unsigned int cluster, int nfat) {
    return (unsigned int *) (disk + (disk_info->BPB_RsvdSecCnt + disk_info->BPB_FATSz32 * (nfat - 1)) * disk_info->BPB_BytsPerSec) + cluster;
}

DirEntry *read_directory(unsigned char *disk, BootEntry *disk_info, unsigned int cluster) {
    return (DirEntry *) (disk + cluster_to_bytes(disk_info, cluster));
}

unsigned char *read_disk(char *disk_name) {
    int fd = open(disk_name, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening disk\n");
        return NULL;
    }

    unsigned char *disk = mmap(NULL, get_file_size(fd), PROT_READ, MAP_SHARED, fd, 0);
    if (disk == MAP_FAILED) {
        fprintf(stderr, "Error reading disk\n");
        return NULL;
    }

    close(fd);
    return disk;
}

int write_disk(char *disk_name, unsigned int root_cluster, unsigned int file_cluster, unsigned int cluster_offset, unsigned char data) {
    int fd = open(disk_name, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error opening disk\n");
        return -1;
    }

    int disk_size = get_file_size(fd);
    if (disk_size < 0)
        return -1;

    unsigned char *disk = mmap(NULL, disk_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (disk == MAP_FAILED) {
        fprintf(stderr, "Error reading disk\n");
        return -1;
    }
    BootEntry *disk_info = (BootEntry *)disk;

    // Update data
    disk[cluster_to_bytes(disk_info, root_cluster) + cluster_offset] = data;

    // Update FAT1
    unsigned int *fat1 = read_fat(disk, disk_info, file_cluster, 1);
    *fat1 = 0x0ffffff8;

    // Update FAT2
    unsigned int *fat2 = read_fat(disk, disk_info, file_cluster, 2);
    *fat2 = 0x0ffffff8;

    close(fd);
    munmap(disk, disk_size);

    return 0;
}
