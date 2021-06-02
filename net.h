#ifndef netH
#define netH

#ifdef __linux__
#include <sys/socket.h>
#include <netdb.h>
#endif

#ifdef WIN32
#include "winsock2.h"
#include "windows.h"
#pragma comment(lib, "ws2_32.lib")
WSADATA wsaData;
#endif
#include <string.h>
#include <time.h>
#include "tools.h"

SOCKET Sock;
struct sockaddr_in inAddr;
struct sockaddr_in serverAddr;

extern unsigned int PORT;
extern char Host[16];
extern char PUBLIC_DNS_IP[16];

enum {
    SERVER = 1,
    CLIENT = 2
};

void init_socket();
char *recvMessage(unsigned int *ip, int *len);
void sendMessage(char *Msg, int len, unsigned int remote);


#endif