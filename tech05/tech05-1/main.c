#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <ctype.h>

struct Item {
  int value;
  uint32_t next_pointer;
};

// Command line arguments:
// 1 - input file
int main(int argc, char** argv) {
	// Open input file
  int fd = open(argv[1], O_RDWR);
  if (-1 == fd) {
		perror("fd");
		_exit(1);
	}
	// Fetch file stats and check if file is empty, terminate if so 
	struct stat statbuf = {};
  fstat(fd, &statbuf);
	if (statbuf.st_size == 0)
		return 0;

	// Map input file to memory
	struct Item *mem = (struct Item*)mmap(NULL, statbuf.st_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED, fd, 0);
	close(fd);
  if (MAP_FAILED == mem) {
    perror("mmap");
    _exit(1);
  }

	// Print values in order defined by next pointers
	printf("%d\n", mem->value);
	uint32_t next = mem->next_pointer;
	while (next != 0) {
		printf("%d\n", (mem + next / sizeof(struct Item))->value);
		next = (mem + next / sizeof(struct Item))->next_pointer;
	}
  
	// Unmap memory
	munmap(mem, statbuf.st_size);
  return 0;
}
