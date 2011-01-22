include ../Makefile.common

NAS= util.o list.o map.o bytecode.o container.o assembly.o assembler.o
NIS= util.o list.o map.o bytecode.o container.o disassembler.o
NVM= util.o list.o map.o bytecode.o container.o job.o x86.o thread.o integer.o vm.o

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
