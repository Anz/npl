CC = gcc --std=99 -Wall

all:
	mkdir -p bin
	${CC} -Iinclude -L../bin -lnbc -lnof -obin/nis src/*.c

clean:
	rm bin
