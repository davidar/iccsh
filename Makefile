VERSION = 0.1
CC = gcc
CFLAGS = -Iinstall/include -Ithirdparty/hashtable -Ithirdparty/cutil/include -Ithirdparty/linenoise -std=gnu99 -DVERSION='"${VERSION}"'
DFLAGS = -g -DDEBUG -rdynamic
XFLAGS = 
LDFLAGS = -Linstall/lib -Lthirdparty/hashtable -Lthirdparty/cutil -ltcc -lhashtable -lcutil -lm -ldl
PROG = iccsh
OBJS = iccsh.o command.o compiler.o memory.o symbols.o token.o linenoise.o
GIT_REPO = git://github.com/davidar/iccsh.git

all: thirdparty ${PROG}

debug:
	$(MAKE) XFLAGS='${DFLAGS}'

test: FORCE
	cd test && ./test.sh

dist:
	git clone ${GIT_REPO} iccsh-${VERSION}
	cd iccsh-${VERSION}/thirdparty && $(MAKE) .submodules
	rm -rf iccsh-${VERSION}/thirdparty/tinycc/win32
	tar -zcvf iccsh-${VERSION}.tar.gz --exclude='.git*' iccsh-${VERSION}
	rm -rf iccsh-${VERSION}

clean:
	rm -f ${PROG} ${OBJS}

thirdparty: FORCE
	cd thirdparty && $(MAKE)

${PROG}: ${OBJS}
	$(CC) ${XFLAGS} -o $@ $^ ${LDFLAGS}

linenoise.o: thirdparty/linenoise/linenoise.c
	$(CC) ${CFLAGS} ${XFLAGS} -c -o $@ $^

.c.o:
	$(CC) ${CFLAGS} ${XFLAGS} -c $<

FORCE:
