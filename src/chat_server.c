#include "../include/server.h"

static void die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

Operations_t get_operation_from_expression(const char *expression) {
  if (strcmp(expression, "ADD") == 0)
    return ADD;
  if (strcmp(expression, "SUB") == 0)
    return SUB;
  if (strcmp(expression, "MUL") == 0)
    return MUL;
  if (strcmp(expression, "DIV") == 0)
    return DIV;

  return INVALID;
}

void evaluate_expression(const char *expression, char *response) {
  char opr[8];
  float num1, num2, result;
  int num_args = sscanf(expression, "%s %f %f", opr, &num1, &num2);

  if (num_args != 3) {
    snprintf(response, BUF_SIZE, "-> ERR: EINV. entrada_invalida\n Uso <OP> <A> <B>\n");
    return;
  }

  Operations_t op = get_operation_from_expression(opr);

  switch (op) {
  case ADD:
    snprintf(response, BUF_SIZE, "-> Ok %.6f\n", num1 + num2);
    break;
  case SUB:
    snprintf(response, BUF_SIZE, "-> Ok %.6f\n", num1 - num2);
    break;
  case MUL:
    snprintf(response, BUF_SIZE, "-> Ok %.6f\n", num1 * num2);
    break;
  case DIV:
    if (num2 != 0) {
      snprintf(response, BUF_SIZE, "-> Ok %.6f\n", num1 / num2);
    } else {
      snprintf(response, BUF_SIZE, "ERR: EZDV divisao_por_zero\n");
    }
    break;
  case INVALID:
    snprintf(response, BUF_SIZE,
             "-> ERR: EINV. entrada_invalida\n Uso <OP> <A> <B>\n");
    break;
  }
}

int main(int argc, char **argv) {
  if (argc > 2) {
    fprintf(stderr,
            "Erro de uso:\n Uso correto:\n %s\n ou \n %s <porta>\n Ex.: %s\n "
            "Ex.: %s 5001\n",
            argv[0], argv[0], argv[0], argv[0]);
    exit(EXIT_FAILURE);
  }

  // Captura para aviso de porta sendo usada
  if (argc == 1) // ./server
    printf("Utilizando porta padrão: %i\n", DEFAULT_PORT);
  else // ./server <porta>
    printf("Utilizando porta: %s", argv[1]);

  int port = (argv[1] == NULL) ? DEFAULT_PORT : atoi(argv[1]);
  if (port <= 0 || port > 65535) {
    fprintf(stderr, "Porta inválida!\n\n");
    return EXIT_FAILURE;
  }

  // socket
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0)
    die("socket");

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons((uint16_t)port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind
  if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    die("bind");

  // listen
  if (listen(listen_fd, 8) < 0)
    die("listen");

  printf("\n***Servidor ouvindo em 0.0.0.0:%d***\n\n", port);

  int clients[MAX_CLIENTS];

  for (int i = 0; i < MAX_CLIENTS; i++)
    clients[i] = -1;

  fd_set allset, rset;
  FD_ZERO(&allset);
  FD_SET(listen_fd, &allset);
  int maxfd = listen_fd;
  int max_i = -1;

  char buf[BUF_SIZE];

  for (;;) {
    rset = allset;
    int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    if (nready < 0) {
      if (errno == EINTR)
        continue;
      die("select");
    }

    if (FD_ISSET(listen_fd, &rset)) {
      struct sockaddr_in cliaddr;
      socklen_t clilen = sizeof(cliaddr);
      int connfd = accept(listen_fd, (struct sockaddr *)&cliaddr, &clilen);
      if (connfd < 0) {
        perror("accept");
      } else {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
        printf("[SERVIDOR] Novo cliente %s:%d (fd=%d)\n", ip,
               ntohs(cliaddr.sin_port), connfd);

        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
          if (clients[i] < 0) {
            clients[i] = connfd;
            break;
          }
        }
        if (i == MAX_CLIENTS) {
          fprintf(stderr, "Muitos clientes, recusando.\n");
          close(connfd);
        } else {
          FD_SET(connfd, &allset);
          if (connfd > maxfd)
            maxfd = connfd;
          if (i > max_i)
            max_i = i;

          const char *welcome = "Bem-vindo!\n";
          send(connfd, welcome, strlen(welcome), 0);
        }
      }
      if (--nready <= 0)
        continue;
    }

    for (int i = 0; i <= max_i; i++) {
      int fd = clients[i];
      if (fd < 0)
        continue;
      if (FD_ISSET(fd, &rset)) {
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) {
          if (n < 0)
            perror("recv");

          printf("[SERVIDOR] Cliente fd=%d desconectou.\n", fd);

          close(fd);
          FD_CLR(fd, &allset);
          clients[i] = -1;

        } else {
          buf[n] = '\0';
          char response[BUF_SIZE];

          evaluate_expression(buf, response);
          send(fd, response, strlen(response), 0);
          printf("[MSG fd=%d] %s", fd, buf);
        }
        if (--nready <= 0)
          break;
      }
    }
  }
  close(listen_fd);
  return 0;
}
