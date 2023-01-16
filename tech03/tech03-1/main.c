#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>

// Command line arguments:
// 1 - input file
int main(int argc, char** argv) {
	// Open input file and create & open output files
  int f_in = open(argv[1], O_RDONLY);
  if (-1 == f_in) {
		perror("f_in");
		_exit(1);
	}
	// Initialize buffers for val and next_pointer
  int val_buff = 0;
  uint32_t next_buff = 0;
  // Check if file is empty, close file and end program if so
  if (0 == read(f_in, &val_buff, sizeof(int))) {
  	close(f_in);
  	return 0;
  }
  // Move offset back to 0
  if (-1 == lseek(f_in, 0, SEEK_SET)) {
  	perror("lseek");
  	_exit(1);
  }
  // Read first val and next_pointer to corresponding buffers, print first val
  if (-1 == read(f_in, &val_buff, sizeof(int))) {
  	perror("read");
  	_exit(1);
  }
  if (-1 == read(f_in, &next_buff, sizeof(uint32_t))) {
  	perror("read");
  	_exit(1);
  }
  printf("%d\n", val_buff);
  // Read items from file and print their vals while end of list is not reached
  while (next_buff != 0) {
  	// Set offset in file corresponding to next_pointer
  	if (-1 == lseek(f_in, next_buff, SEEK_SET)) {
  		perror("lseek");
  		_exit(1);
  	}
  	if (-1 == read(f_in, &val_buff, sizeof(int))) {
  		perror("read");
  		_exit(1);
  	}
  	if (-1 == read(f_in, &next_buff, sizeof(uint32_t))) {
  		perror("read");
  		_exit(1);
  	}
  	printf("%d\n", val_buff);
  } 
  // Close input file
  if (-1 == close(f_in)) {
  	perror("write");
  	_exit(1);
  }
  return 0;
}
