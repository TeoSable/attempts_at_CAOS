#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <stdint.h>
#include <sys/wait.h>

int main() {
  uint64_t count = 1;
  while(1) {
    // Generate new process
    pid_t pid = fork();
    // If unable to create new process, output process count and break cycle
    if (-1 == pid) {
      printf("%ld\n", count);
      break;
    // If current process is newly generated, increment counter and countinue cycle
    } else if (0 == pid) {
      count += 1;
    // If current process is a parent of another process, wait for child process to terminate
    } else {
      waitpid(pid, NULL, 0);
      break;
    }
  }
  return 0;
}