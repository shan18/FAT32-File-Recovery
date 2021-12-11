#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <string.h>
#include "recovery.h"
#include "disk.h"
#include "utils.h"
#define SHA_DIGEST_LENGTH 20
#define print(x) printf("%d\n", x)

int compare_sha1(unsigned char *file_sha, char *sha1) {
    char file_str_sha[SHA_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf(file_str_sha + i * 2, "%02x", file_sha[i]);
    file_str_sha[SHA_DIGEST_LENGTH * 2] = '\0';
    return strcmp(file_str_sha, sha1) == 0;
}

unsigned char *get_sha1(unsigned char *disk, BootEntry *disk_info, DirEntry *entry) {
    unsigned char *file_sha = malloc(SHA_DIGEST_LENGTH);

    unsigned int file_cluster = entry->DIR_FstClusHI << 16 | entry->DIR_FstClusLO;
    unsigned char *content = malloc(file_cluster * disk_info->BPB_SecPerClus * disk_info->BPB_BytsPerSec);
    unsigned int cdone = 0;
    while (cdone < entry->DIR_FileSize / (disk_info->BPB_BytsPerSec * disk_info->BPB_SecPerClus) + 1) {
        memcpy(content + cdone * disk_info->BPB_SecPerClus * disk_info->BPB_BytsPerSec, read_cluster(disk, disk_info, file_cluster), disk_info->BPB_SecPerClus * disk_info->BPB_BytsPerSec);
        file_cluster++;
        cdone++;
    }
    SHA1(content, entry->DIR_FileSize, file_sha);
    free(content);
    return file_sha;
}

void recover(char *disk_name, unsigned int root_cluster, unsigned int root_cluster_offset, DirEntry *entry, unsigned char recovery_data) {
    int disk_size = -1;
    unsigned char *disk = map_disk(disk_name, &disk_size, 'w');
    BootEntry *disk_info = (BootEntry *)disk;

    unsigned int file_cluster = entry->DIR_FstClusHI << 16 | entry->DIR_FstClusLO;
    update_disk(disk, disk_info, root_cluster, root_cluster_offset, recovery_data);

    unsigned int fatc_recovered = 0;
    while (fatc_recovered < entry->DIR_FileSize / (disk_info->BPB_BytsPerSec * disk_info->BPB_SecPerClus)) {
        update_fat(disk, disk_info, file_cluster, file_cluster + 1);
        file_cluster++;
        fatc_recovered++;
    }
    update_fat(disk, disk_info, file_cluster, 0x0ffffff8);

    unmap_disk(disk, disk_size);
}

void recover_contiguous_file(unsigned char *disk, BootEntry *disk_info, char *disk_name, char *file_name, char *sha1) {
    int num_entries = 0, bytes_read = sizeof(DirEntry), file_count = 0;
    unsigned int root_cluster, root_cluster_offset;
    DirEntry *file_entry;
    for (unsigned int next_cluster = disk_info->BPB_RootClus; next_cluster < 0x0ffffff8 && file_count < 2; next_cluster = *read_fat(disk, disk_info, next_cluster, 1)) {
        for (
            DirEntry *entry = read_directory(disk, disk_info, next_cluster);
            entry->DIR_Attr != 0x0 && bytes_read <= disk_info->BPB_BytsPerSec * disk_info->BPB_SecPerClus;
            entry++, bytes_read += sizeof(DirEntry)
        ) {
            if (entry->DIR_Name[0] == 0xe5 && entry->DIR_Attr == 0x20) {
                if (compare_entries(entry->DIR_Name, file_name, 1)) {
                    root_cluster = next_cluster;
                    root_cluster_offset = bytes_read - sizeof(DirEntry);
                    file_entry = entry;
                    if (sha1 != NULL) {
                        unsigned char *file_sha = get_sha1(disk, disk_info, entry);
                        if (compare_sha1(file_sha, sha1)) {
                            file_count = 2;
                            break;
                        }
                        free(file_sha);
                    } else if (++file_count == 2)
                        break;
                }
            }
            num_entries++;
        }
        bytes_read = sizeof(DirEntry);
    }

    if (file_count == 0)
        printf("%s: file not found\n", file_name);
    else if (file_count == 2 && sha1 == NULL)
        printf("%s: multiple candidates found\n", file_name);
    else {
        recover(disk_name, root_cluster, root_cluster_offset, file_entry, (unsigned char) file_name[0]);
        display_entry_name(file_entry->DIR_Name);
        printf(": successfully recovered");
        if (sha1 != NULL)
            printf(" with SHA-1");
        printf("\n");
    }
}