include Makefile.common

LIB_SRC = util.c list.c map.c container.c
NAS_SRC = ${LIB_SRC} assembly.c assembler.c
NIS_SRC = ${LIB_SRC} assembly.c disassembler.c
NVM_SRC = ${LIB_SRC} library.c x86.c thread.c integer.c vm.c

NAS = ${NAS_SRC:%.c=bin/%.o}
NIS = ${NIS_SRC:%.c=bin/%.o}
NVM = ${NVM_SRC:%.c=bin/%.o}

all: bin bin/nas bin/nis bin/nvm

bin:
	mkdir -p bin

bin/nas: ${NAS}
	${CC} ${CFLAGS} -obin/nas ${NAS}

bin/nis: ${NIS}
	${CC} ${CFLAGS} -obin/nis ${NIS}

bin/nvm: ${NVM}
	${CC} ${CFLAGS} -obin/nvm ${NVM} -lpthread

bin/%.o: src/%.c
	${CC} ${CFLAGS} -o${<:src/%.c=bin/%.o} -c $<

clean:
	rm -rf bin
