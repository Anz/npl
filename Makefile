all:
	make -C bin

clean:
	make -C bin clean

as:
	bin/nas test/sample.asm test/object.nbc

dis:
	bin/nis test/object.nbc

vm:
	bin/nvm test/object.nbc

db:
	gdb bin/nvm
