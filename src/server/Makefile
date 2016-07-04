# Makefile for fastsync-server
# 2016-07-01
#   master@pepstack.com
#
###########################################################
# change version[] in server.c by below version:
version=0.0.1

bintarget = fastsync-server
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

$(bintarget): server.c session.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -DCFGDIR="\"$(sysconfdir)\"" \
    ./session.o \
	-o $@ server.c \
	$(LDFLAGS)

session.o: session.c
	$(CC) $(CFLAGS) -c session.c -o $@

clean:
	-rm -f $(binfilename)
	-rm -f session.o

check: all
	@echo "**** ALL TESTS PASSED ****"

install:
	install -d $(prefix)/$(package)/conf
	install -m 0755 $(binfilename) $(prefix)/$(package)/$(binfilename)-$(version)
	install -m 0755 ../../conf/server.xml $(prefix)/$(package)/conf
	install -m 0755 ../../conf/log4crc $(prefix)/$(package)/conf
	ln -s -f $(prefix)/$(package)/$(binfilename)-$(version) $(prefix)/$(package)/$(binfilename)

uninstall:
	@echo "uninstall "$(prefix)"/"$(package)"/"$(binfilename)-$(version)
	-rm -rf $(prefix)/$(package)

.PHONY: all clean check install uninstall
