CC = gcc --std=99 -Wall

all:
	mkdir -p bin
	${CC} -Iinclude -I../libnof/include -I../libnbc/include -L../bin -lnbc -lnof -obin/nas src/*.c

clean:
	rm bin
