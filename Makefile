
all:
	echo ${CFLAGS}
	mkdir -p bin
	mkdir -p bin/include
	make -C libnof
	cp libnof/bin/lib*.a bin/
	cp libnof/include/*.h bin/include/
	make -C libnbc
	cp libnbc/bin/lib*.a bin/
	cp libnbc/include/*.h bin/include/
	make -C nis
	cp nis/bin/nis bin/
	make -C nas
	cp nas/bin/nas bin/

clean:
	rm -rf bin
