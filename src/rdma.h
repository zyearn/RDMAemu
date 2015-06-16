
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) RDMAemu
 */

#ifndef _RDMA_H
#define _RDMA_H

#include <strings.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include "list.h"
#include "rdma_request.h"
#include "dbg.h"

#define RDMA_OK                     0
#define RDMA_PARSE_INVALID_METHOD   10
#define RDMA_SIZE_ERR               100

#define RDMA_AGAIN  EAGAIN

typedef struct rdma_memory_s {
    void *ptr;
    int size;
     
    list_head list;  /* store rdma_memory chain */
} rdma_memory_t;

void rdma_init();
void do_request(void *ptr);

#endif

