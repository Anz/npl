CC = gcc --std=99 -Wall

all:
	mkdir -p bin
	cd bin
	CC -I../include -c ../src/*.c
	ar rcs *.o libnbc.a

clean:
	rm bin
