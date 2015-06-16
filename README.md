libRDMAemu
=====

Visit memory region in another machine.

## start up server

```
cd RDMAemu && make
./objs/rdma_server -c rdma.conf
```

## client usage

```
cd RDMAemu/client_example && make
./example
```

## NOTE

for now, you must configure IP and Port in RDMAemu/example.c
