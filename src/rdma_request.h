
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) RDMAemu
 */

#ifndef _RDMA_REQUEST_H
#define _RDMA_REQUEST_H

#define MAX_BUF 8124

typedef struct rdma_request_s {
    int fd;
    char buf[MAX_BUF];
    int state;
    void *pos, *last;
    void *request_start, *request_end;  // (start, end]

    void *malloc_size_start, *malloc_size_end;

    void *free_ptr_start, *free_ptr_end;

    void *get_start, *get_end;

    void *set_start, *set_end;

    void *end;
} rdma_request_t;

#endif
