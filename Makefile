CC=gcc
CFLAGS=-O2

dns: tools.o dns.o tree.o
	gcc tools.o dns.o tree.o -o dns -lm

clean:
	${RM} *.o dns *.log