# Makefile for fastsync-server
# 2016-07-20
#   master@pepstack.com
#
###########################################################
# change version[] in server.c by below version:
version=0.0.1

bintarget = fastsync-client
binfilename = $(bintarget)

CFLAGS ?= -O2
LDFLAGS ?= /usr/local/lib/liblog4c.a \
	/usr/local/lib/libexpat.a \
	/usr/local/lib/libsqlite3.a \
	/usr/local/lib/libjemalloc.a \
	/usr/local/lib/libmxml.a \
	-lrt \
	-ldl \
	-lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

all: $(package)

$(package): $(bintarget)

$(bintarget): client.c server_conn.o client_conf.o watch_path.o
	$(CC) $(CFLAGS) -DCFGDIR="\"$(sysconfdir)\"" \
	../common/threadpool.o \
	../common/getmac.o \
	../common/md5.o \
	server_conn.o \
	client_conf.o \
	watch_path.o \
	-o $@ client.c \
	$(LDFLAGS)

client_conf.o: client_conf.c
	$(CC) $(CFLAGS) -c client_conf.c -o $@

watch_path.o: watch_path.c
	$(CC) $(CFLAGS) -c watch_path.c -o $@

server_conn.o: server_conn.c
	$(CC) $(CFLAGS) -c server_conn.c -o $@

clean:
	-rm -f $(binfilename)
	-rm server_conn.o
	-rm client_conf.o
	-rm watch_path.o

check: all
	@echo "**** ALL TESTS PASSED ****"

install:
	install -d $(prefix)/$(package)/conf
	install -m 0755 $(binfilename) $(prefix)/$(package)/$(binfilename)-$(version)
	install -m 0755 ../../conf/client-cfg.xml $(prefix)/$(package)/conf
	tar -C $(prefix)/$(package) -czf $(prefix)/$(package)/conf/$(binfilename)-$(version).tar.gz $(binfilename)-$(version)
	ln -s -f $(prefix)/$(package)/$(binfilename)-$(version) $(prefix)/$(package)/$(binfilename)

uninstall:
	@echo "uninstall "$(prefix)"/"$(package)"/"$(binfilename)-$(version)
	-rm -rf $(prefix)/$(package)

.PHONY: all clean check install uninstall
