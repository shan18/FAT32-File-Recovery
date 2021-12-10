#ifndef _RECOVERY_
#define _RECOVERY_

#include "disk.h"

void recover_contiguous_file(unsigned char *disk, BootEntry *disk_info, char *disk_name, char *file_name, char *sha1);

#endif