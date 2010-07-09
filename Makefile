CC = gcc
CFLAGS = -Iinstall/include -Ithirdparty/hashtable -Ithirdparty/cutil/include -std=gnu99
DFLAGS = -g -DDEBUG
XFLAGS = 
LDFLAGS = -Linstall/lib -Lthirdparty/hashtable -Lthirdparty/cutil -ltcc -lhashtable -lcutil -lm -ldl
PROG = iccsh
OBJS = iccsh.o command.o symbols.o compiler.o token.o

all: thirdparty ${PROG}

debug:
	$(MAKE) XFLAGS='${DFLAGS}'

clean:
	rm -f ${PROG} ${OBJS}

thirdparty: FORCE
	cd thirdparty && $(MAKE)

${PROG}: ${OBJS}
	$(CC) ${XFLAGS} -o $@ $^ ${LDFLAGS}

.c.o:
	$(CC) ${CFLAGS} ${XFLAGS} -c $<

FORCE:
