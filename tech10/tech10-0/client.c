#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    exit(1);
  }

  long port = strtol(argv[2], NULL, 10);

  struct sockaddr_in address = {};

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  int text_to_address = inet_pton(AF_INET, argv[1], &address.sin_addr);
  if (text_to_address < 1) {
    perror("inet_pton");
    exit(1);
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    perror("socket");
    exit(1);
  }
  int connection_indicator = connect(sockfd, (struct sockaddr*) &address, sizeof(address));
  if (-1 == connection_indicator) {
    perror("connect");
    exit(1);
  }

  int query = 0;
  int res = 0;
  unsigned char c;

  while (0 != recv(sockfd, &c, 1, MSG_DONTWAIT)) {  
    int scanned = scanf("%d", &query);
    if (EOF == scanned) {
      int shutdown_code = shutdown(sockfd, SHUT_RDWR);
      if (shutdown_code == -1) {
        perror("shutdown");
        exit(1);
      }
      break;
    }  
    ssize_t write_b = write(sockfd, &query, sizeof(query));
    if (write_b == -1) {
      perror("write");
      exit(1);
    }
    int read_status = read(sockfd, &res, sizeof(res));
    if (-1 == read_status) {
      perror("read");
      exit(1);
    }
    if (0 == read_status) {
      break;
    }
    printf("%d\n", res);
  }

  int close_code = close(sockfd);
  if (close_code == -1) {
    perror("close");
    exit(1);
  }
  return 0;
}