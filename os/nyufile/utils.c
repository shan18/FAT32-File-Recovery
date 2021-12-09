#include <stdio.h>
#include "utils.h"

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