include ../Makefile.common

CFLAGS+= -c

all:
	mkdir -p bin
	for file in `cd src; ls *.c | sed s/\.c$//g`; do ${CC} -obin/$$file.o ${CFLAGS} src/$$file.c; done
	ar rs bin/libnof.a bin/*.o

clean:
	rm bin
