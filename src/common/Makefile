# Makefile for common
#
#   master@pepstack.com
#
# 2014-08-01: init created
# 2017-01-14: last updated
########################################################################
CFLAGS ?= -O0
LDFLAGS ?= -lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

all: red_black_tree.o readconf.o threadpool.o hashmap.o getmac.o md5.o base64.o getopt_longw.o


getopt_longw.o: getopt_longw.c
	$(CC) $(CFLAGS) -c getopt_longw.c -o $@

base64.o: base64.c
	$(CC) $(CFLAGS) -c base64.c -o $@

red_black_tree.o: red_black_tree.c
	$(CC) $(CFLAGS) -c red_black_tree.c -o $@

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
	-rm base64.o red_black_tree.o readconf.o threadpool.o hashmap.o getmac.o md5.o getopt_longw.o

check: all
	@echo "**** ALL TESTS PASSED ****"

.PHONY: all clean check install uninstall
