#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

// Function that launches cmd using fd_in as input source and fd_out as output destination
// If fd_in or fd_out are -1, the corresponding standart stream is not duplicated
pid_t launch(const char *cmd, int fd_in, int fd_out) {
  pid_t pid = fork();
  if (-1 == pid) {
    perror("fork");
    _exit(1);
  }
  if (0 == pid) {
    if (-1 != fd_in) {
      if (-1 == dup2(fd_in, STDIN_FILENO)) {
        return -1;
      }
      close(fd_in);
    }

    if (-1 != fd_out) {
      if(-1 == dup2(fd_out, STDOUT_FILENO)) {
        return -1;
      }
      close(fd_out);
    }
    
    execlp(cmd, cmd, NULL);
    exit(2);
  } 
  else {
    waitpid(pid, NULL, 0);
    return pid;
  }
}

// Command line arguments:
// 1-N - CMD1-CMDN
int main(int argc, const char *argv[]) {
  int N = argc - 1;
  if (N == 0) {
    return 0;
  }
  if (N == 1) {
    launch(argv[1], -1, -1);
    return 0;
  }
  // Create descriptor array for (N - 1) pipes
  int* pipe_fd = (int*)malloc(2 * (N - 1) * sizeof(int));
  // Pipe first two descriptors and launch CMD1 with standard input and pipe output
  pipe(pipe_fd);
  launch(argv[1], -1, pipe_fd[1]);
  close(pipe_fd[1]);
  for (int i = 1; i < N - 1; ++i) {
    // Make new pipe, launch next CMD with output from previous pipe and redirect CMD output to new pipe
    pipe(pipe_fd + 2 * i);
    launch(argv[i + 1], pipe_fd[2 * (i - 1)], pipe_fd[2 * i + 1]);
    // Close the reading end of previous pipe and writing end of new one
    close(pipe_fd[2 * (i - 1)]);
    close(pipe_fd[2 * i + 1]);
  }
  // Launch last command with last pipe output and redirect its output to stdout
  launch(argv[N], pipe_fd[2 * (N - 2)], -1);
  close(pipe_fd[2 * (N - 2)]);

  return 0;
}