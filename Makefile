include Makefile.common

LIB_SRC = util.c list.c map.c container.c
NAS_SRC = ${LIB_SRC} assembly.c assembler.c
NIS_SRC = ${LIB_SRC} assembly.c disassembler.c
NVM_LIB_SRC = library.c integer.c array.c string.c stdout.c conversion.c
NVM_SRC = ${LIB_SRC} ${NVM_LIB_SRC:%.c=lib/%.c} x86.c thread.c vm.c

NAS = ${NAS_SRC:%.c=bin/%.o}
NIS = ${NIS_SRC:%.c=bin/%.o}
NVM = ${NVM_SRC:%.c=bin/%.o}

all: bin bin/nas bin/nis bin/nvm

install: all
	@cp bin/nas /usr/bin/
	@cp bin/nis /usr/bin/
	@cp bin/nvm /usr/bin/

uninstall:
	@rm /usr/bin/nas /usr/bin/nis /usr/bin/nvm

bin: bin/lib
	@mkdir -p bin

bin/lib:
	@mkdir -p bin/lib

bin/nas: ${NAS}
	@echo "LD nas"
	@${CC} ${CFLAGS} -obin/nas ${NAS}

bin/nis: ${NIS}
	@echo "LD nis"
	@${CC} ${CFLAGS} -obin/nis ${NIS}

bin/nvm: ${NVM}
	@echo "LD nvm"
	@${CC} ${CFLAGS} -obin/nvm ${NVM} -lm -lpthread

bin/%.o: src/%.c
	@echo "CC $<"
	@${CC} ${CFLAGS} -o${<:src/%.c=bin/%.o} -c $<

bin/lib/%.o: src/lib/%.c
	@echo "CC $<"
	@${CC} ${CFLAGS} -o${<:src/%.c=bin/%.o} -c $<

clean:
	@rm -rf bin
