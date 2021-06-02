CC=gcc
CFLAGS=-O2 -g

dns: tools.c dns.c tree.c net.c main.c
	gcc tools.c dns.c tree.c net.c main.c -o dns -lm -lws2_32 -g

clean:
	${RM} *.o dns *.log