# Makefile for fastsync
# 2016-07-01
#   master@pepstack.com
#
###########################################################

package = fastsync
version = 0.0.1
tarname = $(package)
distdir = $(tarname)-$(version)

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

export prefix
export exec_prefix
export bindir

export package

CUR_DIR = .
CONF_DIR = $(CUR_DIR)/conf
SRC_DIR = $(CUR_DIR)/src
COMMON_DIR = $(SRC_DIR)/common
CLIENT_DIR = $(SRC_DIR)/client
SERVER_DIR = $(SRC_DIR)/server

all clean check install uninstall isynclog:
	cd $(SERVER_DIR) && $(MAKE) $@

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir): FORCE
	mkdir -p $(distdir)/src/common
	mkdir -p $(distdir)/src/client
	mkdir -p $(distdir)/src/server
	mkdir -p $(distdir)/conf
	cp Makefile $(distdir)
	cp $(SRC_DIR)/*.h $(distdir)/src
	cp $(COMMON_DIR)/* $(distdir)/src/common
	cp $(CLIENT_DIR)/* $(distdir)/src/client
	cp $(SERVER_DIR)/* $(distdir)/src/server
	cp $(CONF_DIR)/client.xml $(distdir)/conf
	cp $(CONF_DIR)/server.xml $(distdir)/conf
	cp $(CONF_DIR)/log4crc $(distdir)/conf

distcheck: $(distdir).tar.gz
	gzip -cd $(distdir).tar.gz | tar xvf -
	cd $(distdir) && $(MAKE) all
	cd $(distdir) && $(MAKE) check
	cd $(distdir) && $(MAKE) DESTDIR=$${PWD}/_inst install
	cd $(distdir) && $(MAKE) DESTDIR=$${PWD}/_inst uninstall
	@remaining="`find $${PWD}/$(distdir)/_inst -type f | wc -l`"; \
	if test "$${remaining}" -ne 0; then \
		echo "*** $${remaining} file(s) remaining in stage directory!"; \
		exit 1; \
	fi
	cd $(distdir) && $(MAKE) clean
	rm -rf $(distdir)
	@echo "**** Package $(distdir).tar.gz is ready for distribution. ****"

FORCE:
	-rm $(distdir).tar.gz >/dev/null 2>&1
	-rm -rf $(distdir) >/dev/null 2>&1

.PHONY: FORCE all clean check dist distcheck install uninstall
