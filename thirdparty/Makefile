CWD = $(shell pwd)
INSTALL_DIR = ${CWD}/../install

all:
	cd .. && git submodule update --init
	cd cutil && $(MAKE)
	cd hashtable && $(MAKE)
	cd tinycc && ./configure --prefix=${INSTALL_DIR} && $(MAKE) all install

clean:
	cd cutil && $(MAKE) clean
	cd hashtable && $(MAKE) clean
	cd tinycc && $(MAKE) clean
	rm -rf ${INSTALL_DIR}
