CC=gcc
CFLAGS=

dns: main.o dns.o tree.o net.o tools.o trie.o log.o link.o
	gcc main.o net.o dns.o tools.o tree.o trie.o log.o link.o -o dns -lm -lws2_32

clean:
	${RM} *.o dns *.log dns.exe

run: main.o dns.o tree.o net.o tools.o trie.o log.o link.o
	gcc main.o net.o dns.o tools.o tree.o trie.o log.o link.o -o dns -lm -lws2_32
	dns.exe

linux: main.o dns.o tree.o net.o tools.o trie.o log.o link.o
	gcc main.o net.o dns.o tools.o tree.o trie.o log.o link.o -o dns -lm