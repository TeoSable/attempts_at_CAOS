#include <signal.h>
#include <unistd.h>
#include <stdio.h>

// Indicator for reaching N == 0
volatile sig_atomic_t sigrtmin = 1;

// Define handler for SIGRTMIN that automatically sends signal with (N - 1) back to sender
void process_sigrtmin(int signum, siginfo_t *info, void *ucontext) {
  if (info->si_value.sival_int == 0) {
    sigrtmin = 0;
    return;
  } else {
    --info->si_value.sival_int;
    int ret_val = sigqueue(info->si_pid, SIGRTMIN, info->si_value);
    if (-1 == ret_val) {
      perror("sigqueue");
      _exit(1);
    }
  }
}

int main() {
  // Set user-defined handler for SIGRTMIN
  struct sigaction sigrtmin_action = {};
  sigrtmin_action.sa_sigaction = &process_sigrtmin;
  sigrtmin_action.sa_flags = SA_SIGINFO;

  int sigaction_code = sigaction(SIGRTMIN, &sigrtmin_action, NULL);
  if (-1 == sigaction_code) {
    perror("sigaction:SIGRTMIN");
    _exit(1);
  }

  for (int i = SIGRTMIN + 1; i <= SIGRTMAX; ++i) {
    signal(i, SIG_IGN);
  }

  pid_t pid; int N = 0;
  scanf("%d\n", &pid);
  scanf("%d\n", &N);
  fflush(stdin);
  union sigval sgval = {};
  sgval.sival_int = N; sgval.sival_ptr = NULL;
  int ret_val = sigqueue(pid, SIGRTMIN, sgval);
  if (-1 == ret_val) {
    perror("sigqueue");
    _exit(1);
  }

  while (1) {
    if (!sigrtmin) {
      break;
    }
    pause();
  }
  return 0;
}
