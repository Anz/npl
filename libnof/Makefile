CC = gcc --std=99 -Wall

all:
	mkdir -p bin
	cd bin
	CC -Iinclude -c ../src/*.c
	ar rcs *.o libnof.a

clean:
	rm bin
