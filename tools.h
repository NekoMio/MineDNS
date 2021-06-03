#ifndef toolsH
#define toolsH
#include "dns.h"
#include <stdarg.h>
#include <stdio.h>

extern unsigned int PORT;
extern char Host[16];
extern char PUBLIC_DNS_IP[16];
extern int DD;

unsigned short unPackFlags(unsigned short flags, int type);
unsigned short SetFlags(unsigned short flags, int type, unsigned char val);
unsigned short decodeHeader(char *Query, DNSHeader *queryheader);
char *encodeName(char *name);
char *decodeQuestion(char *Query, DNSQuestion *queryquestion);
unsigned short packData(DNSHeader *Header, DNSQuestion *Question, char *RR,
                        int RRlen, char *servername, char *Response,
                        int servernamelen);
unsigned char findInStatic(char *name, unsigned int *x);
unsigned short deleteMap(unsigned short x, unsigned int *ip, unsigned short *port);
unsigned short createMap(unsigned short x, unsigned int ip, unsigned short port);
void LOG(int type, char *format, ...);

typedef struct IDMapStruct{
    unsigned int ip;
    unsigned short ID;
    unsigned short port;
}IDMap;

IDMap IDMapData[65536];

unsigned short IDMapQueue[65537];
int IDMapQueueLeft, IDMapQueueRight;

typedef enum {
    NORMALMSG = 1,
    WARNINGMSG = 2,
    ERRORMSG = 4,
    DEBUGMSG = 8,
    LOGMSG = 16
} LOGTYPE;

#endif