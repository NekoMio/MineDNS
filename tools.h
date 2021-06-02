#ifndef toolsH
#define toolsH
#include "dns.h"
#include <stdarg.h>
#include <stdio.h>
unsigned short unPackFlags(unsigned short flags, int type);
unsigned short SetFlags(unsigned short flags, int type, unsigned char val);
unsigned short decodeHeader(char *Query, DNSHeader *queryheader);
char *encodeName(char *name);
char *decodeQuestion(char *Query, DNSQuestion *queryquestion);
unsigned short packData(DNSHeader *Header, DNSQuestion *Question, char *RR,
                        int RRlen, char *servername, char *Response,
                        int servernamelen);
unsigned char findInStatic(char *name, unsigned int *x);
unsigned short deleteMap(unsigned short x, int *ip);
unsigned short createMap(unsigned short x, int ip);
void LOG(int type, ...);

typedef struct IDMapStruct{
    unsigned int ip;
    unsigned short ID;
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