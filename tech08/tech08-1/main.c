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
    return -1;
  } 
  else {
    waitpid(pid, NULL, 0);
    return pid;
  }
}

// Command line arguments:
// 1 - CMD1
// 2 - CMD2
int main(int argc, const char *argv[]) {
  if (argc != 3) {
    _exit(1);
  }

  int pipe_fd[2];
  pipe(pipe_fd);
  launch(argv[1], -1, pipe_fd[1]);
  close(pipe_fd[1]);
  launch(argv[2], pipe_fd[0], -1);
  close(pipe_fd[0]);

  return 0;
}