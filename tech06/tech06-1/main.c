#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  int N = atoi(argv[1]);
  for (int i = 1; i < N; ++i) {
    pid_t pid = fork();
    if (-1 == pid) {
      perror("fork");
      _exit(1);
    }
    if (pid == 0) {
      printf("%d ", i);
      return 0;
    } else {
      waitpid(pid, NULL, 0);
    }
  }
  printf("%d\n", N);
  return 0;
}