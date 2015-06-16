#include "rdma_client.h"

int main(int argc, char *argv[]) {
	int rc;

	/* connect */
    rc = RDMA_connect("192.168.1.102", 3000);
	check(rc > 0, "RDMA_connect");
	log_info("connect remote successfully!");

	/* ask available */
	char buf[BUF_SIZE];
	memset(buf, '\0', sizeof(buf));

	rc = RDMA_ask_available(buf, BUF_SIZE);
	check(rc == 0, "RDMA_ask_available");
	log_info("RDMA_ask_available successfully!");

	log_info("After ask, remote memory infomation is: %s", buf);
	log_info("First Number representing how many memory regions which are followed with initial ptr and size");
	log_info("If you want more memory, you should call RDMA_MALLOC");

	/* malloc in remote */
	rc = RDMA_malloc(100);
	check(rc == 0, "RDMA_malloc");
	log_info("RDMA_malloc successfully!");

	log_info("now you can ask remote infomation again");

	/* ask available */
	rc = RDMA_ask_available(buf, BUF_SIZE);
	check(rc == 0, "RDMA_ask_available");
	log_info("RDMA_ask_available successfully!");
	log_info("After ask, remote memory infomation is: %s", buf);
	log_info("First Number representing how many memory regions which are followed with initial ptr and size");
	log_info("Now we can find there is one memory region");
	log_info("We can set and get value from this region");

	/* set */
	int number, size;
	void *ptr;
	sscanf(buf, "%d %p %d", &number, &ptr, &size);
	debug("number=%d", number);
	debug("ptr=%p", ptr);
	debug("size=%d", size);
	
	rc = RDMA_set(ptr, 12, "012345678912");
	check(rc == 0, "RDMA_SET");
	log_info("RDMA_set successfully!");
	log_info("then we can get the data that we previously set");

	/* get */
	memset(buf, '\0', sizeof(buf));
	rc = RDMA_get(ptr, 12, buf);
	check(rc == 0, "RDMA_GET");
	log_info("RDMA_get successfully!");
	log_info("the data we previously set is %s", buf[2]);	// the format is "Y <data>"

	return 0;
}
