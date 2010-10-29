CC = gcc --std=99 -Wall

all:
	mkdir -p bin
	cd bin
	CC -I../include -c ../src/*.c
	ar rcs *.o libnof.a

clean:
	rm bin
