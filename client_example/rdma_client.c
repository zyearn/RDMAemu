#include "rdma_client.h"

static int connfd = -1;

int RDMA_connect(char *ip, int port) {
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd != -1);

    if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        printf ("connection failed\n");
        return -1;
    } else {
		connfd = sockfd;
        return sockfd;
    }
}

int RDMA_ask_available(char *result, int size) {
	if (connfd < 0) {
		log_err("call RDMA_connect first");
		return -1;
	}

	write(connfd, "ASK\r\n", 5);
	read(connfd, result, BUF_SIZE);

	return 0;
}

int RDMA_malloc(int size) {
	if (connfd < 0) {
		log_err("call RDMA_connect first");
		return -1;
	}

	if (size < 0) {
		log_err("size must greater than 0");
		return -1;
	}

	char buf[BUF_SIZE];
	sprintf(buf, "MALLOC %d\r\n\0", size);
	debug("malloc buf = %s", buf);
	write(connfd, buf, strlen(buf));
	read(connfd, buf, BUF_SIZE);

	if (buf[0] == 'Y') {
		return 0;
	} else {
		return -1;
	}
}


int RDMA_set(void *ptr, int size, void *buffer) {
	if (connfd < 0) {
		log_err("call RDMA_connect first");
		return -1;
	}

	if (size < 0) {
		log_err("size must greater than 0");
		return -1;
	}

	char buf[BUF_SIZE];
	memset(buf, '\0', BUF_SIZE);
	sprintf(buf, "SET %p %d ", ptr, size);
	strncat(buf, buffer, size);
	strncat(buf, "\r\n", 2);
	debug("set buf = %s", buf);

	write(connfd, buf, strlen(buf));
	read(connfd, buf, BUF_SIZE);

	if (buf[0] == 'Y') {
		return 0;
	} else {
		return -1;
	}
}


int RDMA_get(void *ptr, int size, void *buffer) {
	if (connfd < 0) {
		log_err("call RDMA_connect first");
		return -1;
	}

	if (size < 0) {
		log_err("size must greater than 0");
		return -1;
	}
	
	char buf[BUF_SIZE];
	memset(buf, '\0', BUF_SIZE);

	debug("p %p, d %d", ptr, size);
	sprintf(buf, "GET %p %d\r\n", ptr, size);
	debug("get buf = %s", buf);

	write(connfd, buf, strlen(buf));
	read(connfd, buffer, size + 10);

	if (buf[0] == 'Y') {
		return 0;
	} else {
		return -1;
	}

}
