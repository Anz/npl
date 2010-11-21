include ../Makefile.common

LDFLAGS+= -lnbc -lnof

all:
	mkdir -p bin
#	${CC} -Iinclude -I../libnof/include -I../libnbc/include -L../bin -lnbc -lnof -obin/nas src/*.c
	${CC} ${CFLAGS} -obin/nas src/*.c ${LDFLAGS}

clean:
	rm bin
