
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) RDMAemu
 */

#include "rdma.h"
#include "rdma_request.h"
#include "list.h"


static int rdma_size = 0;
static list_head rdma_head;

void rdma_init() {
    INIT_LIST_HEAD(&rdma_head);
}

void do_request(void *ptr) {
    rdma_request_t *r = (rdma_request_t *)ptr;
    int fd = r->fd;
    int rc;
    int n;
    
    for(;;) {
        n = read(fd, r->last, (uint64_t)r->buf + MAX_BUF - (uint64_t)r->last);
        check((uint64_t)r->buf + MAX_BUF > (uint64_t)r->last, "(uint64_t)r->buf + MAX_BUF");
        debug("check");

        if (n == 0) {   // EOF
            debug("n==0");
            log_info("read return 0, ready to close fd %d", fd);
            goto err;
        }

        if (n < 0) {
            debug("n<0");
            if (errno != EAGAIN) {
                log_err("read err, and errno = %d", errno);
                goto err;
            }
            break;
        }

        debug("check");
        r->last += n;
        check(r->last <= r->buf + MAX_BUF, "r->last <= MAX_BUF");
        
        log_info("ready to parse request line"); 
        rc = rmda_parse_request(r);
        log_info("return from parse");
        if (rc == RDMA_AGAIN) {
            continue;
        } else if (rc != RDMA_OK) {
            log_err("rc != RDMA_OK");
            goto err;
        }

        rc = serve_rdma(r);
        log_info("return from serve_rdma");
    }
    
    debug("ready return");
    return;

err:
close:
    //free(ptr);
    close(fd);
}

int serve_rdma(rdma_request_t *r) {
    char buf[MAX_BUF];
    memset(buf, 0, MAX_BUF);

    log_info("in the serve_rdma");
    /* ASK */
    if (strncmp(r->request_start, "ASK", r->request_end - r->request_start) == 0) {
        log_info("ASK from remote");
        
        sprintf(buf, "%d", rdma_size);
        int i=0;
        list_head *pos;
        rdma_memory_t *hd;

        list_for_each(pos, &rdma_head) {
            hd = list_entry(pos, rdma_memory_t, list);

            /* delete it from the original list
            list_del(pos);
            */

            sprintf(buf, "%s %p %d", buf, hd->ptr, hd->size);
        }
        return rio_writen(r->fd, buf, strlen(buf));
    }

    /* MALLOC */
    if (strncmp(r->request_start, "MALLOC", r->request_end - r->request_start) == 0) {
        log_info("MALLOC from remote");
        int size;
        sscanf(r->malloc_size_start, "%d", &size);

        log_info("size = %d", size);
        if (size < 0) {
            goto MALLOC_err; 
        }

        rdma_memory_t *nrdma = (rdma_memory_t *)malloc(sizeof(rdma_memory_t));
        if (nrdma == NULL) {
            goto MALLOC_err; 
        }
        void *ptr = malloc(size);
        nrdma->ptr = ptr;
        nrdma->size = size;
        list_add(&(nrdma->list), &rdma_head);
        rdma_size++;

        return rio_writen(r->fd, "Y", 1);

        MALLOC_err:

        return rio_writen(r->fd, "N", 1);
    }
    
    /* SET */
    if (strncmp(r->request_start, "SET", r->request_end - r->request_start) == 0) {
        log_info("SET from remote");
    
        void *ptr;
        int size;
        *((char *)r->set_end) = '\0';
        sscanf(r->set_start, "%p %d %s", &ptr, &size, buf);

        debug("p %p", ptr);
        debug("size %d", size);
        debug("buf %s", buf);

        list_head *pos;
        rdma_memory_t *hd;

        list_for_each(pos, &rdma_head) {
            hd = list_entry(pos, rdma_memory_t, list);

            if (hd->ptr <= ptr && hd->ptr + hd->size > ptr) {
                if (ptr+size < hd->ptr + hd->size) {
                    memcpy(ptr, buf, size);
                    return rio_writen(r->fd, "Y", 1);
                } else {
                    return rio_writen(r->fd, "N", 1);
                }

                break;
            }
        }
        
        return rio_writen(r->fd, "N", 1);
    }

    if (strncmp(r->request_start, "GET", r->request_end - r->request_start) == 0) {
        log_info("GET from remote");

        void *ptr;
        int size;
        sscanf(r->get_start, "%p %d", &ptr, &size);

        debug("p %p", ptr);
        debug("size %d", size);

        list_head *pos;
        rdma_memory_t *hd;

        list_for_each(pos, &rdma_head) {
            hd = list_entry(pos, rdma_memory_t, list);

            if (hd->ptr <= ptr && hd->ptr + hd->size > ptr) {
                if (ptr+size < hd->ptr + hd->size) {
                    sprintf(buf, "Y ");
                    strncat(buf, ptr, size);
                    return rio_writen(r->fd, buf, strlen(buf));
                } else {
                    return rio_writen(r->fd, "N", 1);
                }

                break;
            }
        }



        return rio_writen(r->fd, "N", 1);
    }

    debug("ready to return from serve rdma");
    return rio_writen(r->fd, "1", 1);
}
