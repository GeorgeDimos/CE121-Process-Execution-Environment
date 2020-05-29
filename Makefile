CC= gcc
CFLAGS= -Wall -g

final: all .PHONY

all: hw4.o util.o 
	$(CC) $(CFLAGS) hw4.o util.o -o hw4

hw4.o: hw4.c
	$(CC) $(CFLAGS) -c hw4.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

.PHONY: clean
clean: 
	rm -rf *.o