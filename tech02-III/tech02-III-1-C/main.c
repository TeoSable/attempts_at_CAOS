#include <sys/syscall.h>
#include <linux/limits.h>
#include <unistd.h>

extern long syscall(long number, ...);

void _start() {
  char text;
  ssize_t bytes_read = 0;
  // Read up to page size bytes from standart input. If error code is returned, exit with code 2.
  // Repeat until EOF is reached, indicated by return code 0 
  do {
    bytes_read = syscall(SYS_read, STDIN_FILENO, &text, 1);
    if (bytes_read == -1) {
      syscall(SYS_exit, 2);
    }
    // Write text to standart output. Exit with code 3 in case of error
    ssize_t bytes_written = syscall(SYS_write, STDOUT_FILENO, &text, 1);
    if (bytes_written == -1)
      syscall(SYS_exit, 3);
  } while (bytes_read > 0);

  // Exit with code 0 upon success
  syscall(SYS_exit, 0);
}