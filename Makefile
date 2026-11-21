obj-m += kmsgbuf.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

.PHONY: all module client clean
all: module

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

client: kmsgbufctl

kmsgbufctl: kmsgbufctl.c
	$(CC) -Wall -Wextra -Wpedantic -O2 -o $@ $<

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) kmsgbufctl
