#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#define LONG_LONG_MAX_DIGITS 19

// SIGUSR1, SIGUSR2, SIGINT and SIGTERM indicators
volatile sig_atomic_t sigusr1 = 0;
volatile sig_atomic_t sigusr2 = 0;
volatile sig_atomic_t sigint = 0;
volatile sig_atomic_t sigterm = 0;

// Define handlers for SIGUSR1, SIGUSR2, SIGINT and SIGTERM
void process_sigusr1(int signum) {
  sigusr1 = 1;
}
void process_sigusr2(int signum) {
  sigusr2 = 1;
}
void process_sigint(int signum) {
  sigint = 1;
}
void process_sigterm(int signum) {
  sigterm = 1;
}

// Neither stdio's standard output functions nor snprintf or itoa are async-signal-safe,
// so we are forced to write our own function for printing integers
ssize_t write_num(long long number) {
  ssize_t bytes = 0;
  if (number == 0) {
    const char zero[2] = "0\n";
    bytes = write(STDOUT_FILENO, zero, 2);
    return bytes;
  }
  if (number < 0) {
    const char minus[1] = "-";
    bytes = write(STDOUT_FILENO, minus, 1);
    if (bytes == -1) {
      return -1;
    } else {
      number *= -1;
    }
  }
  unsigned int log = 0;
  long long num_tmp = number;
  while (num_tmp) {
    ++log;
    num_tmp /= 10;
  }
  char number_str[LONG_LONG_MAX_DIGITS + 1] = "";
  for (unsigned int i = 0; i < log; ++i) {
    number_str[log - 1 - i] = '0' + (number % 10);
    number /= 10;
  }
  number_str[log] = '\n';
  bytes += write(STDOUT_FILENO, number_str, log + 1);
  return bytes;
}

int main() {
  long long num = 0;

  // Set user-defined handlers for SIGUSR1, SIGUSR2, SIGINT and SIGTERM
  struct sigaction sigusr1_action = {};
  struct sigaction sigusr2_action = {};
  struct sigaction sigint_action = {};
  struct sigaction sigterm_action = {};

  sigusr1_action.sa_handler = process_sigusr1;
  sigusr2_action.sa_handler = process_sigusr2;
  sigint_action.sa_handler = process_sigint;
  sigterm_action.sa_handler = process_sigterm;

  int sigaction_code = sigaction(SIGUSR1, &sigusr1_action, NULL);
  if (-1 == sigaction_code) {
    perror("sigaction:sigusr1");
    _exit(1);
  }
  sigaction_code = sigaction(SIGUSR2, &sigusr2_action, NULL);
  if (-1 == sigaction_code) {
    perror("sigaction:sigusr2");
    _exit(1);
  }
  sigaction_code = sigaction(SIGINT, &sigint_action, NULL);
  if (-1 == sigaction_code) {
    perror("sigaction:sigint");
    _exit(1);
  }
  sigaction_code = sigaction(SIGTERM, &sigterm_action, NULL);
  if (-1 == sigaction_code) {
    perror("sigaction:sigterm");
    _exit(1);
  }
  ssize_t bytes_written = 0;

  printf("%d\n", getpid());
  fflush(stdout);
  scanf("%lld", &num);
  fflush(stdin);

  while (1) {
    if (sigusr1) {
      sigusr1 = 0;
      ++num;
      bytes_written = write_num(num);
      if (bytes_written <= 0) { // Exit if write fails
        _exit(1);
      }
    }
    if (sigusr2) {
      sigusr2 = 0;
      num *= -1;
      bytes_written = write_num(num);
      if (bytes_written <= 0) {
        _exit(1);
      }
    }
    if (sigint) {
      return 0;
    }
    if (sigterm) {
      return 0;
    }

    pause();
  }
}
