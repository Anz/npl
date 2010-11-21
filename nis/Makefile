include ../Makefile.common

LDFLAGS+= -lnbc -lnof

all:
	mkdir -p bin
	echo "ROOTDIR  ### $(CURDIR) - ${CURDIR}"
	${CC} ${CFLAGS} -obin/nis src/*.c ${LDFLAGS}

clean:
	rm bin
