#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define BUFF_SIZE 4096
#define SIGNAL_COUNT 64
// SIGUSR1, SIGUSR2, SIGINT and SIGTERM indicators
volatile sig_atomic_t signals[SIGNAL_COUNT + 1]; // +1 for convenient indexation

// Define handlers for SIGUSR1, SIGUSR2, SIGINT and SIGTERM
void process_signal(int signum) {
  printf("Caught signal %d\n", signum);
  fflush(stdout);
  signals[signum] = 1;
}

// Command line arguments:
// argc - N
// argv[1 - N] - names of files from 1 to N
int main(int argc, char *argv[]) {
  // Set user-defined handler for all signals from SIGRTMIN to SIGRTMAX
  struct sigaction signal_action = {};
  signal_action.sa_handler = process_signal;

  for (int i = 1; i <= SIGRTMAX; ++i) {
    // printf("argc = %d\n", argc);
    int sigaction_code = sigaction(i, &signal_action, NULL);
    if (-1 == sigaction_code) {
      if (errno == EINVAL)
        continue;
      perror("sigaction");
      exit(1);
    }
  }
  char *buff = NULL; 
  size_t size = 0;
  ssize_t read = 0;
  FILE *in_file[argc + 1]; // +1 for convenient indexation
  for (int i = 1; i < argc; ++i) {
    in_file[i] = fopen(argv[i], "r");
    if (NULL == in_file[i]) {
      perror("fopen");
      exit(1);
    }
  }
        
  // printf("%d\n", getpid());
  // fflush(stdout);

  while (1) {
    if (signals[SIGRTMIN]) {
      for (int i = 1; i < argc; ++i) {
        fclose(in_file[i]);
      }
      return 0;
    }
    for (int i = 1; i < argc; ++i) {
      if (signals[i + SIGRTMIN] != 0) {
        // printf("Caught signal %d\n", i + SIGRTMIN);
        fflush(stdout);
        read = getline(&buff, &size, in_file[i]);
        if (-1 != read) {
          printf("%s", buff);
          fflush(stdout);
        }
      }
    }
    for (int i = 1; i <= SIGRTMAX; ++i) {
      signals[i] = 0;
    }
    pause();
  }
}
