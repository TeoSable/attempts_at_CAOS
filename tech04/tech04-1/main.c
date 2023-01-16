#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

enum CONSTS {
  MAX_NAME_SIZE = 256,
  BUFF_SIZE = 40000,
  MAX_INPUT = 2048
};

int main() {
  // Create buffers for wrong file names, suggested interpreter path input, read input, file name input, file size and file stats
  char wrong_files[BUFF_SIZE] = "";
  char intrpr_path[MAX_INPUT] = "";
  char input[MAX_INPUT] = "";
  char* f_name;
  struct stat f_stat;
  // Declare correct executable file indicator, counter for interpreter path buffer, variable to store line size in getline, 
  // current position in wrong file names buffer and bytes read from file
  int correct = 0;
  int counter = 0;
  size_t n = 0;
  size_t cur_pos = 0;
  ssize_t bytes = 0;
  // Read lines from std input until EOF
  ssize_t bytes_read = getline(&f_name, &n, stdin);
  while (bytes_read > 0) {
    // Delete \n from end of input string
    if (f_name[bytes_read - 1] == '\n') {
      f_name[bytes_read - 1] = 0;
    }
    // Check if file exists and is executable
    if (access(f_name, F_OK) == 0 && access(f_name, X_OK) == 0) {
      // Open file for reading
      int fd = open(f_name, O_RDONLY);
      if (-1 == fd) {
        perror("open");
        _exit(1);
      }
      // Read first 4 bytes of file
      bytes = read(fd, input, MAX_INPUT);
      if (-1 == bytes) {
        perror("read");
        _exit(1);
      }
      // Check if first four bytes exist; if they are an ELF signature, mark file as correct
      if (bytes >= 4) {
        if (input[0] == 0x7f && input[1] == 0x45 && input[2] == 0x4c && input[3] == 0x46)
          correct = 1;
        // Otherwise check if first two bytes are "#!"; copy line after them to interpreter buffer if so
        else if (input[0] == '#' && input[1] == '!') {
          counter = 2;
          while (input[counter] != '\n' && input[counter] != 0 && counter < MAX_INPUT) {
            intrpr_path[counter - 2] = input[counter];
            ++counter;
          }
          // Check if interpreter is correct
          if (access(intrpr_path, X_OK) == 0 && access(intrpr_path, F_OK) == 0)
            correct = 1;
          // Clean interpreter buffer and reset counter
          for (int i = 0; i < counter - 2; ++i) {
            intrpr_path[i] = 0;
          }
          counter = 0;
        }  
      }
      // If file isn't flagged as correct, write its name to wrong names buffer
      if (correct == 0) {
        for (int i = 0; i < bytes_read - 1; ++i) {
          *(wrong_files + cur_pos + i) = f_name[i];
        }
        cur_pos += bytes_read;
        wrong_files[cur_pos - 1] = '\n';
      }
      correct = 0;
      // Close opened file
      if (-1 == close(fd)) {
        perror("write");
        _exit(1);
      }
    } 
    // Free dynamically allocated memory
    free(f_name);
    n = 0;
    bytes_read = getline(&f_name, &n, stdin); 
  }
  // Print wrong file names buffer
  printf("%s", wrong_files);
  return 0;
}