#include <signal.h>
#include <unistd.h>
#include <stdio.h>

// Initialize SIGINT and SIGTERM indicators
volatile sig_atomic_t sigint = 0;
volatile sig_atomic_t sigterm = 0;

// Define handlers for SIGINT and SIGTERM
void process_sigint(int signum) {
  sigint = 1;
}
void process_sigterm(int signum) {
  sigterm = 1;
}

int main() {
  unsigned int counter = 0;

  // Set user-defined handlers for SIGINT and SIGTERM
  struct sigaction sigint_action = {};
  struct sigaction sigterm_action = {};

  sigint_action.sa_handler = process_sigint;
  sigterm_action.sa_handler = process_sigterm;

  int sigaction_code_1 = sigaction(SIGINT, &sigint_action, NULL);
  int sigaction_code_2 = sigaction(SIGTERM, &sigterm_action, NULL);
  if (-1 == sigaction_code_1 || -1 == sigaction_code_2) {
    perror("sigaction");
    _exit(1);
  }

  printf("%d\n", getpid());
  fflush(stdout);

  while (1) {
    if(sigint) {
      sigint = 0;
      counter++;
    }

    if (sigterm) {
      printf("%u\n", counter);
      return 0;
    }

    pause();
  }
}