
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) RDMAemu
 */

#include "rdma.h"
#include "rdma_request.h"
#include "util.h"

int rdma_init_request_t(rdma_request_t *r, int fd, zv_conf_t *cf){
    r->fd = fd;
    r->pos = r->last = r->buf;
    r->state = 0;

    return RDMA_OK;
}
