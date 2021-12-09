#include <stdio.h>
#include <sys/stat.h>
#include "utils.h"

void display_usage() {
    puts("Usage: ./nyufile disk <options>");
    puts("  -i                     Print the file system information.");
    puts("  -l                     List the root directory.");
    puts("  -r filename [-s sha1]  Recover a contiguous file.");
    puts("  -R filename -s sha1    Recover a possibly non-contiguous file.");
}

int get_file_size(int fd) {
    struct stat stat;
    if (fstat(fd, &stat) == -1) {
        fprintf(stderr, "Invalid input file");
        return -1;
    }
    return stat.st_size;
}

void display_entry_name(unsigned char *entry) {
    // Display name
    for(int i = 0; i <= 7; i++) {
        if(entry[i] != ' ')
            printf("%c", entry[i]);
    }
    
    // Display '.' if the entry has an extension
    if(entry[8] != ' ')
        printf(".");

    // Display extension
    for(int i = 8; i < 11; i++) {
        if(entry[i] != ' ')
            printf("%c", entry[i]);
    }
}