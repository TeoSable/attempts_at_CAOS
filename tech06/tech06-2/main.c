#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/wait.h>
#include <string.h>

int main() {
  int count = -1;
  int exit_status = 0;
  char word[4096];
  while (exit_status == 0) {
    ++count;
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      _exit(1);
    }
    if (pid == 0) {
      int res = scanf("%s", word);
      if (res == EOF) {
        return 1;
      } else {
        memset(word, 0, sizeof(word));
        return 0;
      }
    } else {
      waitpid(pid, &exit_status, WEXITSTATUS(exit_status));
    }
  }
  printf("%d\n", count);
  return 0;
}