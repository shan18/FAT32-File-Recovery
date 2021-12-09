#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"

int get_file_size(int fd) {
    struct stat stat;
    if (fstat(fd, &stat) == -1) {
        fprintf(stderr, "Invalid input file");
        return -1;
    }
    return stat.st_size;
}

BootEntry *read_disk(char *disk_name) {
  int fd = open(disk_name, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error opening disk\n");
    return NULL;
  }

  int file_size = get_file_size(fd);
  if (file_size == -1) {
      fprintf(stderr, "Invalid disk\n");
      return NULL;
  }

  BootEntry *disk = mmap(0, sizeof(BootEntry), PROT_READ, MAP_SHARED, fd, 0);
  if (disk == MAP_FAILED) {
    fprintf(stderr, "Error reading disk\n");
    return NULL;
  }

  close(fd);

  return disk;
}
