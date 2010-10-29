CC = gcc --std=99 -Wall

all:
	mkdir -p bin
	${CC} -I../include -L../bin -lnbc -lnof -obin/nas src/*.c

clean:
	rm bin
