include ../Makefile.common

NAS= util.o assembly.o bytecode.o container.o map.o list.o assembler.o
NIS= util.o bytecode.o container.o map.o list.o disassembler.o
NVM= util.o bytecode.o container.o job.o map.o x86.o thread.o integer.o vm.o

all: nas nis nvm

nas: ${NAS}
	${CC} ${CFLAGS} -onas ${NAS}

nis: ${NIS}
	${CC} ${CFLAGS} -onis ${NIS}

nvm: ${NVM}
	${CC} ${CFLAGS} -onvm ${NVM} -lpthread

%.o: ../src/%.c
	${CC} ${CFLAGS} -c $<

clean:
	rm -rf nis
	rm -rf nas
	rm -rf nvm
	rm -rf *.o
