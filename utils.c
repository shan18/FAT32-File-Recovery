#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

void display_usage() {
    puts("Usage: ./run disk <options>");
    puts("  -i                     Print the file system information.");
    puts("  -l                     List the root directory.");
    puts("  -r filename [-s sha1]  Recover a contiguous file.");
}

int get_file_size(int fd) {
    struct stat stat;
    if (fstat(fd, &stat) == -1) {
        fprintf(stderr, "Invalid input file");
        return -1;
    }
    return stat.st_size;
}

char *get_packed_entry(unsigned char *entry) {
    char *packed_entry = malloc(sizeof(char) * 12);
    int j = 0;
    for(int i = 0; i <= 7; i++) {
        if (entry[i] != ' ')
            packed_entry[j++] = entry[i];
    }

    if(entry[8] != ' ')
        packed_entry[j++] = '.';

    for(int i = 8; i < 11; i++) {
        if(entry[i] != ' ')
            packed_entry[j++] = entry[i];
    }

    packed_entry[j] = '\0';
    return packed_entry;
}

void display_entry_name(unsigned char *entry) {
    char *packed_entry = get_packed_entry(entry);
    printf("%s", packed_entry);
    free(packed_entry);
}

int compare_entries(unsigned char *dir_entry, char *target_entry, int start) {
    char *packed_entry = get_packed_entry(dir_entry);
    int result = strcmp(packed_entry + start, target_entry + start) == 0;
    free(packed_entry);
    return result;
}