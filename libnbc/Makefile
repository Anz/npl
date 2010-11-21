include ../Makefile.common

CFLAGS+= -c

all:
	mkdir -p bin
#	cd bin;	${CC} -I../include -c ../src/*.c
	for file in `cd src; ls *.c | sed s/\.c$//g`; do ${CC} -obin/$$file.o ${CFLAGS} src/$$file.c; done
	ar rs bin/libnbc.a bin/*.o
#	ar rs bin/libnbc.a bin/*.o 

clean:
	rm bin
