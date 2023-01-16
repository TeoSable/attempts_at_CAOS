#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

volatile sig_atomic_t sigterm = 0;
void process_sigterm(int signum) {
  sigterm = 1;
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    exit(1);
  }

  struct sigaction sigterm_action = {};
  sigterm_action.sa_handler = process_sigterm;
  if (-1 == sigaction(SIGTERM, &sigterm_action, NULL)) {
    perror("sigaction:sigterm");
    exit(1);
  }

  const int EPOLL_QUEUE = 64;
  struct epoll_event events[EPOLL_QUEUE];
  int epfd = epoll_create(1);
  if (-1 == epfd) {
    perror("epoll_create");
    exit(1);
  }

  long port = strtol(argv[1], NULL, 10);
  struct sockaddr_in address = {};
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    exit(1);
  }
  if (-1 == bind(sockfd, (const struct sockaddr*) &address, sizeof(address))) {
    perror("bind");
    exit(1);
  }
  if (-1 == listen(sockfd, SOMAXCONN)) {
    perror("listen");
    exit(1);
  }

  while(!sigterm) {
    struct sockaddr_in client_addr = {};
    socklen_t addr_len;
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd == -1) {
      if (errno = EINTR) {
        continue;
      }
      perror("accept");
      exit(1);
    }
    int fd_flags = fcntl(client_fd, F_GETFL);
    if (fd_flags == -1) {
      perror("fcntl");
      exit(1);
    }
    fd_flags |= O_NONBLOCK;
    if (-1 == fcntl(client_fd, F_SETFL, fd_flags)) {
      perror("fcntl");
      exit(1);
    }
    events[0].data.fd = client_fd;
    events[0].events = EPOLLIN;
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, events)) {
      perror("epoll_ctl");
      exit(1);
    }

    char data[PATH_MAX];
    int evcount = epoll_wait(epfd, events, EPOLL_QUEUE, 1);
    if (-1 == evcount) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("epoll_wait");
        exit(1);
      }
    } else if (0 == evcount) {
      continue;
    } else {
      for (int i = 0; i < evcount; ++i) {
        int read_b = 0;
        while((read_b = read(events[i].data.fd, data, PATH_MAX))) {
          if (-1 == read_b) {
            if (errno == EWOULDBLOCK || errno == EINTR) {
              break;
            } else {
              perror("read");
              exit(1);
            }
          } else {
            for (int k = 0; k < read_b; ++k) {
              if (data[k] <= 'z' && data[k] >= 'a') {
                data[k] -= 32;
              }
            }
            if (-1 == write(events[i].data.fd, data, read_b)) {
              if (errno == EINTR) {
                break;
              }
              perror("write");
              exit(1);
            }
            if (!read_b) {
              if (-1 == shutdown(events[i].data.fd, SHUT_RDWR)) {
                perror("shutdown");
                exit(1);
              }
              if (-1 == close(events[i].data.fd)) {
                perror("close");
                exit(1);
              }
            }
          }
          if (errno == EINTR) {
            break;
          }
        }
      }
    }
  }
  if (-1 == shutdown(sockfd, SHUT_RDWR)) {
    perror("shutdown");
    exit(1);
  }
  if (-1 == close(sockfd)) {
    perror("close");
    exit(1);
  }
  return 0;
}