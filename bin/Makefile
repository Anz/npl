include ../Makefile.common

NAS= util.o assembly.o bytecode.o container.o assembler.o
NIS= util.o bytecode.o container.o disassembler.o

all: nas nis

nas: ${NAS}
	${CC} ${CFLAGS} -onas ${NAS}

nis: ${NIS}
	${CC} ${CFLAGS} -onis ${NIS}

%.o: ../src/%.c
	${CC} ${CFLAGS} -c $<

clean:
	rm -rf nis
	rm -rf nas
	rm -rf *.o
