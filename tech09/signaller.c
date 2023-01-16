#include <signal.h>
#include <stdio.h>

int main() {
  pid_t pid;
  scanf("%d\n", &pid);
  const union sigval val = {};
  int new_signal;
  scanf("%d\n", &new_signal);
  while (new_signal != 0) {
    sigqueue(pid, new_signal, val);
    scanf("%d\n", &new_signal);
  }
}