#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <signal.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

volatile sig_atomic_t sigint = 0;
volatile sig_atomic_t sigterm = 0;
void process_sigint(int signum) {
  sigint = 1;
}
void process_sigterm(int signum) {
  sigterm = 1;
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    exit(1);
  }
  struct sigaction sigint_action = {};
  struct sigaction sigterm_action = {};
  sigint_action.sa_handler = process_sigint;
  sigterm_action.sa_handler = process_sigterm;
  if (-1 == sigaction(SIGINT, &sigint_action, NULL)) {
    perror("sigaction:sigint");
    exit(1);
  }
  if (-1 == sigaction(SIGTERM, &sigterm_action, NULL)) {
    perror("sigaction:sigterm");
    exit(1);
  }

  long port = strtol(argv[1], NULL, 10);
  struct sockaddr_in address = {};
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    perror("socket");
    exit(1);
  }
  int bind_code = bind(sockfd, (const struct sockaddr*) &address, sizeof(address));
  if (-1 == bind_code) {
    perror("bind");
    exit(1);
  }
  int listen_code = listen(sockfd, SOMAXCONN);
  if (-1 == listen_code) {
    perror("listen");
    exit(1);
  }

  while(1) {
    if (sigint || sigterm) {
      break;
    }
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

    int query_len = 0;
    char query[PATH_MAX];
    // char lines[PATH_MAX];
    query_len = read(client_fd, &query, PATH_MAX * sizeof(char));
    // while (1 < read(client_fd, &lines, PATH_MAX * sizeof(char))) {}
    if (query_len <= sizeof("GET  HTTP/1.1")) {
      goto SHUT_DOWN_CLIENT;
    }

    char filename[PATH_MAX];
    if (NULL == strncpy(filename, query + 4, strstr(query, " HTTP/1.1") - query - 4)) {
      perror("strncpy");
      goto SHUT_DOWN_CLIENT;
    }
    char full_filename[PATH_MAX * 2];
    snprintf(full_filename, PATH_MAX * 2, "%s/%s", argv[2], filename);
    // if (0 >= fprintf(stderr, "%s", filename)) { // 
    //   perror("printf");                // DEBUG
    //   goto SHUT_DOWN_CLIENT;           //
    // }                                  //
    if (-1 == access(full_filename, F_OK)) {
      const char msg[] = "HTTP/1.1 404 Not Found\r\n";
      if (-1 == write(client_fd, msg, sizeof(msg))) {
        perror("write");
        goto SHUT_DOWN_CLIENT;
      }
    } else if (-1 == access(full_filename, R_OK)) {
      const char msg[] = "HTTP/1.1 403 Forbidden\r\n";
      if (-1 == write(client_fd, msg, sizeof(msg))) {
        perror("write");
        goto SHUT_DOWN_CLIENT;
      }
    } else {
      int r_fd = open(full_filename, O_RDONLY);
      char msg[PATH_MAX];
      struct stat stats = {};
      if (-1 == fstat(r_fd, &stats)) {
        perror("fstat");
        goto SHUT_DOWN_CLIENT;
      }
      int msg_len = snprintf(msg, PATH_MAX, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", (int)stats.st_size);
      if (-1 == write(client_fd, msg, msg_len)) {
        perror("write");
        goto SHUT_DOWN_CLIENT;
      }
      char r_buff[PATH_MAX];
      ssize_t read_b = 1;
      while (read_b) {
        read_b = read(r_fd, r_buff, PATH_MAX);
        if (-1 == write(client_fd, r_buff, read_b)) {
          perror("write");
          goto SHUT_DOWN_CLIENT;
        }
      }
      close(r_fd);
    }
    SHUT_DOWN_CLIENT: // shut down
      if (-1 == shutdown(client_fd, SHUT_RDWR)) {
        if (errno == ENOTCONN)
          continue;
        perror("shutdown");
        exit(1);
      }
      int close_code = close(client_fd);
      if (close_code == -1) {
        perror("close");
        exit(1);
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