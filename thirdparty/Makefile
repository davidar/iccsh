CWD = $(shell pwd)
INSTALL_DIR = ${CWD}/../install
LIBS = cutil/libcutil.a hashtable/libhashtable.a ${INSTALL_DIR}/lib/libtcc.a

all: .submodules ${LIBS}

.submodules:
	cd .. && git submodule update --init
	touch .submodules

cutil/libcutil.a:
	cd cutil && $(MAKE)

hashtable/libhashtable.a:
	cd hashtable && $(MAKE)

${INSTALL_DIR}/lib/libtcc.a:
	cd tinycc && ./configure --prefix=${INSTALL_DIR} && $(MAKE) all install

clean:
	cd cutil && $(MAKE) clean
	cd hashtable && $(MAKE) clean
	cd tinycc && $(MAKE) clean
	rm -rf ${INSTALL_DIR}
