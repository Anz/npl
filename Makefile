all:
	mkdir -p bin
	cd libnof; make
	cp libnof/bin/lib*.a bin/
	cd libnbc; make
	cp libnbc/bin/lib*.a bin/
	cd nis; make
	cp nis/bin/nis bin/
	cd nas; make
	cp nas/bin/nas bin/
	
