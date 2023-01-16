#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
  // Create buffers for file name, file size and file stats
  char* f_name;
  size_t n = 0;
  struct stat f_stat;
  off_t sum = 0;
  int eof_detected = 0;
  // Read lines from std input until EOF
  ssize_t bytes_read = getline(&f_name, &n, stdin);
  while (bytes_read > 0) {
    // Delete \n from end of input string
    if (f_name[bytes_read - 1] == '\n') {
      f_name[bytes_read - 1] = 0;
    }
    // Get file stats
    if (-1 == lstat(f_name, &f_stat)) {
      perror("lstat");
      _exit(1);
    }
    // Add file size to sum if it is regular
    if ((f_stat.st_mode & S_IFMT) == S_IFREG) {
      sum += f_stat.st_size;
    }
    // Free dynamically allocated memory
    free(f_name);
    n = 0;
    bytes_read = getline(&f_name, &n, stdin); 
  }
  free(f_name);
  printf("%ld", sum);
  return 0;
}
