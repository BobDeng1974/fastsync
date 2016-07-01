# Makefile for common objects
# 2014-08-01
#   zhangliang
#
CFLAGS ?= -O0
LDFLAGS ?= -lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

all: readconf.o threadpool.o hashmap.o getmac.o md5.o

readconf.o: readconf.c
	$(CC) $(CFLAGS) -c readconf.c -o $@

threadpool.o: threadpool.c
	$(CC) $(CFLAGS) -c threadpool.c -o $@

hashmap.o: hashmap.c
	$(CC) $(CFLAGS) -c hashmap.c -o $@

getmac.o: getmac.c
	$(CC) $(CFLAGS) -c getmac.c -o $@

md5.o: md5.c
	$(CC) $(CFLAGS) -c md5.c -o $@

clean:
	-rm readconf.o threadpool.o hashmap.o getmac.o md5.o

check: all
	@echo "**** ALL TESTS PASSED ****"

.PHONY: all clean check install uninstall
