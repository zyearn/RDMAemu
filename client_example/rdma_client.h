#ifndef _RDMA_CLIENT_H
#define _RDMA_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/dbg.h"

#define BUF_SIZE 4096

int RDMA_connect(char *ip, int port);
int RDMA_ask_available(char *result, int size);
int RDMA_malloc(int size);
int RDMA_set(void *ptr, int size, void *buffer);
int RDMA_get(void *ptr, int size, void *buffer);


#endif
