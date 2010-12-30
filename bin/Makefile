include ../Makefile.common

NAS= util.o assembly.o bytecode.o container.o assembler.o
NIS= util.o bytecode.o container.o disassembler.o
VM= util.o bytecode.o container.o job.o x86.o thread.o vm.o

all: nas nis vm

nas: ${NAS}
	${CC} ${CFLAGS} -onas ${NAS}

nis: ${NIS}
	${CC} ${CFLAGS} -onis ${NIS}

vm: ${VM}
	${CC} ${CFLAGS} -onvm ${VM} -lpthread

%.o: ../src/%.c
	${CC} ${CFLAGS} -c $<

clean:
	rm -rf nis
	rm -rf nas
	rm -rf nvm
	rm -rf *.o
