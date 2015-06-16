
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) RDMAemu
 */

#ifndef _RDMA_PARSE_H
#define _RDMA_PARSE_H

#include "rdma_request.h"
#include "rdma.h"

#define CR '\r'
#define LF '\n'
#define CRLFCRLF "\r\n\r\n"

int rmda_parse_request(rdma_request_t *r);

#endif
