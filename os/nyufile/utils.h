#ifndef _UTILS_
#define _UTILS_

void display_usage();
void display_entry_name(unsigned char *entry);
int get_file_size(int fd);
int compare_entries(unsigned char *dir_entry, char *target_entry, int start);

#endif