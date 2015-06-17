libRDMAemu
=====

Visit memory region in another machine.

## start up server

```
cd RDMAemu && make
./objs/rdma_server -c rdma.conf
```

## client usage

Before compiling client program, you must configure IP and Port in RDMAemu/example.c

```
cd RDMAemu/client_example && make
./example
```

