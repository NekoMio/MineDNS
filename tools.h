#ifndef toolsH
#define toolsH
#include <stdarg.h>
#include <stdio.h>
#include "net.h"

#include "dns.h"

extern unsigned int PORT;
extern char Host[16];
extern char PUBLIC_DNS_IP[16];
extern int DD;

unsigned short unPackFlags(unsigned short flags, int type);
unsigned short SetFlags(unsigned short flags, int type, unsigned char val);
unsigned short decodeHeader(char *Query, DNSHeader *queryheader);
char *encodeName(char *name);
char *decodeQuestion(char *Query, DNSQuestion *queryquestion, int *len);
unsigned short packData(DNSHeader *Header, DNSQuestion *Question, char *RR,
                        int RRlen, char *servername, char *Response,
                        int servernamelen);
unsigned char searchinCache(char *name, unsigned short type, char **RR,
                            int *len);
void addtoCache(char *name, unsigned short type, time_t ttl, char *RR,
                int *len);
int deleteMap(unsigned short x, unsigned int *ip, unsigned short *port);
unsigned short createMap(unsigned short x, unsigned int ip,
                         unsigned short port);
unsigned int unpackforttl(char *Query, unsigned short num);

typedef struct IDMapStruct {
  unsigned int ip;
  unsigned short ID;
  unsigned short port;
} IDMap;

IDMap IDMapData[65536];

unsigned short IDMapQueue[65537];
int IDMapQueueLeft, IDMapQueueRight;

#endif