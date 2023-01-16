#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

size_t str_size(char* str) {
  size_t size = 0;
  while(str[size] != '\0')
    ++size;
  return size;
}

int main (int argc, char** argv) {
  int fd = open(argv[1], O_RDWR);
  if (-1 == fd) {
    perror("open");
    _exit(1);
  }
  struct stat statbuf = {};
  fstat(fd, &statbuf);

  char *str = mmap(NULL, statbuf.st_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED, fd, 0);
  close(fd);
  if (MAP_FAILED == str) {
    perror("mmap");
    _exit(1);
  }
  size_t size = str_size(argv[2]);
  int pos = 0;
  while (pos < statbuf.st_size) {
    if (strncmp(str + pos, argv[2], size) == 0)
      printf("%d\n", pos);
    ++pos;
  }

  munmap(str, statbuf.st_size);
  return 0;
}