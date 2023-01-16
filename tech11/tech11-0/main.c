#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

size_t read_data_and_count(size_t N, int in[]) {
  if (0 == N) return 0;
  const int EPOLL_QUEUE = 64;
  char data[PATH_MAX];
  struct epoll_event events[EPOLL_QUEUE];

  int epfd = epoll_create(N);
  if (-1 == epfd) {
    perror("epoll_create");
    exit(1);
  }

  for (int i = 0; i < N; i++) {
    int fd_flags = fcntl(in[i], F_GETFL);
    if (fd_flags == -1) {
      perror("fcntl");
      exit(1);
    }
    fd_flags |= O_NONBLOCK;
    if (-1 == fcntl(in[i], F_SETFL, fd_flags)) {
      perror("fcntl");
      exit(1);
    }

    events[0].data.fd = in[i];
    events[0].events = EPOLLIN;
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, in[i], events)) {
      perror("epoll_ctl");
      exit(1);
    }
  }

  size_t total = 0;
  int closed = 0;

  while (closed < N) {
    int evcount = epoll_wait(epfd, events, EPOLL_QUEUE, -1);
    if (evcount == -1) {
      perror("epoll_wait");
      exit(1);
    }
    for (int i = 0; i < evcount; i++) {
      int read_b = 0;
      while(read_b = read(events[i].data.fd, data, PATH_MAX)) {
        if (-1 == read_b) {
          if (errno == EWOULDBLOCK) {
            break;
          } else {
            perror("read");
            exit(1);
          }
        }

        total += read_b;
      }

      if (!read_b) {
        closed++;
        close(events[i].data.fd);
      }
    }
  }

  return total;
}
