#include <sys/syscall.h>
#include <unistd.h>

extern long syscall(long number, ...);

void _start() {
  // Define "Hello, world!" string
  const char str[] = "Hello, World!";
  // Write string to standart output
  ssize_t bytes = syscall(SYS_write, 1, str, sizeof(str) - 1);
  // Exit with code 0 or 1, depending on SYS_write return code
  if (bytes >= 0) {
    syscall(SYS_exit, 0);
  } else {
    syscall(SYS_exit, 1);
  }
}
