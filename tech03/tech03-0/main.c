#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

// Arguments:
// 1 - input file name
// 2 - output file 1 name
// 3 - output file 2 name
int main(int argc, char** argv) {
	// Open input file and create & open output files
  int f_in = open(argv[1], O_RDONLY);
  if (-1 == f_in) {
		perror("f_in");
		_exit(1);
	}
  int f_out_1 = open(argv[2], O_WRONLY | O_CREAT, 0666);
  if (-1 == f_out_1) {
		perror("f_out_1");
		_exit(2);
	}
  int f_out_2 = open(argv[3], O_WRONLY | O_CREAT, 0666);
  if (-1 == f_out_2) {
		perror("f_out_2");
		_exit(2);
	}
  // Create buffers for input/output
  const size_t buff_cap = 4096;
  char buff_in[buff_cap];
  char buff_out_1[buff_cap];
  char buff_out_2[buff_cap];
  size_t buff_in_len = 0;
  size_t buff_out_1_len = 0;
  size_t buff_out_2_len = 0;
  // Read input file
  while (buff_in_len = read(f_in, buff_in, buff_cap)) {
    for (size_t i = 0; i < buff_in_len; ++i) {
    	// Check if char from input is a digit; if so, write to output buffer 1, write to output buffer 2 otherwise
      if (isdigit(buff_in[i])) {
        buff_out_1[buff_out_1_len++] = buff_in[i];
      }
      else {
        buff_out_2[buff_out_2_len++] = buff_in[i];
      }
      // If any output buffer is full, write buffer to corresponding output file and empty buffer
      if (buff_out_1_len == buff_cap) {
        if (-1 == write(f_out_1, buff_out_1, buff_cap)) {
        	perror("write");
        	_exit(3);
        }
        buff_out_1_len = 0;
      }
      if (buff_out_2_len == buff_cap) {
        if (-1 == write(f_out_2, buff_out_2, buff_cap)) {
        	perror("write");
        	_exit(3);
        }
        buff_out_2_len = 0;
      }
    }
  }
  // If error code is returned by read, exit
  if (buff_in_len == -1) {
  	perror("read");
  	_exit(3);
  }
  // Write output buffers to corresponding output files
  if (-1 == write(f_out_1, buff_out_1, buff_out_1_len)) {
  	perror("write");
  	_exit(3);
  }
  if (-1 == write(f_out_2, buff_out_2, buff_out_2_len)) {
  	perror("write");
  	_exit(3);
  } 
  // Close all files
  int fds[3] = { f_in, f_out_1, f_out_2 };
  for (int i = 0; i < 3; ++i) {
  	if (-1 == close(fds[i])) {
  		perror("close");
  		_exit(3);
  	}
  }
  return 0;
}
