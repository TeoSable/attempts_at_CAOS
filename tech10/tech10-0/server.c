#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, const char *argv[]) {
  if (argc != 2) {
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
    struct sockaddr_in client_addr = {};
    socklen_t addr_len;
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd == -1) {
      perror("accept");
      exit(1);
    }

    int has_read = 1;
    int num = 0;

    char addr_str [INET_ADDRSTRLEN] = {}; //  Buffer for string representation of address
    const char *address_to_text = inet_ntop(AF_INET,       // Domain
                                    &client_addr.sin_addr, // Pointer to address
                                    addr_str,              // Pointer to address string buffer
                                    sizeof(addr_str)       // Buffer size
                                   );
    if (address_to_text == NULL) {
      perror("inet_ntop");
      exit(1);
    }

    for (int i = 0; i < 3; i++) {
      has_read = read(client_fd, &num, sizeof(num));
      if (has_read <= 0) {
        break;
      }

      printf("CLIENT: %s, RECIEVED: %d\n", addr_str, num);

      num += 1;
      ssize_t write_b = write(client_fd, &num, sizeof(num));
      if (write_b == -1) {
        perror("write");
        exit(1);
      }

      printf("CLIENT: %s, SENDING: %d\n\n", addr_str, num);
    }

    int shutdown_code = shutdown(client_fd, SHUT_RDWR);
    if (shutdown_code == -1) {
      perror("shutdown");
      exit(1);
    }
    int close_code = close(client_fd);
    if (close_code == -1) {
      perror("close");
      exit(1);
    } 
  }
}