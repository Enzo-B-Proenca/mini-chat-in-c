#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SERVER_H

#define SERVER_H
#define MAX_CLIENTS FD_SETSIZE
#define BUF_SIZE 1024
#define DEFAULT_PORT 5050

typedef enum {
  ADD,
  SUB,
  MUL,
  DIV,
  INVALID
} Operations_t; 

static void die(const char *msg);
Operations_t get_operation_from_expression(const char *expression);
void evaluate_expression(const char *expression, char *response);

#endif
