CC=gcc
CFLAGS=-g

dns: main.o dns.o tree.o net.o tools.o trie.o log.o 
	gcc main.o net.o dns.o tools.o tree.o trie.o log.o -o dns -lm -lws2_32 -g

clean:
	${RM} *.o dns *.log

run: main.o dns.o tree.o net.o tools.o trie.o log.o
	gcc main.o net.o dns.o tools.o tree.o trie.o log.o -o dns -lm -lws2_32 -g
	dns.exe