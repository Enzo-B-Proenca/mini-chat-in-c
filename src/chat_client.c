#include "../include/client.h"

static void die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  const char *server_ip;
  int port;
  if (argc > 3 || argc == 2) {
    fprintf(stderr,
            "Erro de uso!\n Formatos corretos:\n %s\n %s <ip-servidor> "
            "<porta>\n Ex.: "
            "%s 127.0.0.1 5000\n ",
            argv[0], argv[0], argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 1) {
    server_ip = DEFAULT_IP;
    port = DEFAULT_PORT;
    printf("Utilizando porta e IP padrão: %s %i\n", DEFAULT_IP, DEFAULT_PORT);
  } else {
    server_ip = argv[1];
    port = atoi(argv[2]);
    printf("Utilizando IP : porta %s : %s\n", argv[1], argv[2]);
  }

  if (port <= 0 || port > 65535) {
    fprintf(stderr, "Porta inválida.\n");
    return EXIT_FAILURE;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    die("Socket");

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons((uint16_t)port);

  if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
    fprintf(stderr, "IP inválido: %s\n", server_ip);
    close(sockfd);
    return EXIT_FAILURE;
  }

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    die("Connect");

  printf("[CLIENTE] Conectado em %s:%d\n", server_ip, port);
  printf("[CLIENTE] Digite mensagens e pressione ENTER. Ctrl+D para sair.\n");

  fd_set rset;
  char sendbuf[BUF_SIZE];
  char recvbuf[BUF_SIZE];

  for (;;) {
    FD_ZERO(&rset);
    FD_SET(STDIN_FILENO, &rset);
    FD_SET(sockfd, &rset);
    int maxfd = (STDIN_FILENO > sockfd ? STDIN_FILENO : sockfd);

    // select espera até ter dados no teclado ou no socket
    int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    if (nready < 0) {
      if (errno == EINTR)
        continue;
      die("select");
    }

    if (FD_ISSET(STDIN_FILENO, &rset)) {
      if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL) {
        printf("\n[CLIENTE] EOF do teclado. Encerrando.\n");
        break;
      }
      size_t len = strlen(sendbuf);
      if (len > 0) {
        ssize_t n = send(sockfd, sendbuf, len, 0);
        if (n < 0) {
          perror("send");
          break;
        }
      }
    }

    if (FD_ISSET(sockfd, &rset)) {
      ssize_t n = recv(sockfd, recvbuf, sizeof(recvbuf) - 1, 0);
      if (n <= 0) {
        if (n < 0)
          perror("recv");
        printf("[CLIENTE] Servidor encerrou a conexão.\n");
        break;
      }
      recvbuf[n] = '\0';
      printf("%s", recvbuf);
      fflush(stdout);
    }
  }

  close(sockfd);

  return EXIT_SUCCESS;
}
