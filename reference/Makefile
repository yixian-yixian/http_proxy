src = $(wildcard *.c)
obj = $(src:.c=.o)
CC = gcc
LDFLAGS = -lnsl

a.out: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

tcpserver: tcpserver.c 
	$(CC) -o $@ $^ $(LDFLAGS)



.PHONY: clean
clean:
	rm -f $(obj) a.out core.* vgcore.*