#ifndef toolsH
#define toolsH
#include "dns.h"
unsigned short unPackFlags(unsigned short flags, int type);
void SetFlags(unsigned short flags, int type, unsigned char val);
unsigned short decodeHeader(char *Query, DNSHeader *queryheader);
char *encodeName(char *name);
char *decodeQuestion(char *Query, DNSQuestion *queryquestion);
unsigned short packData(DNSHeader *Header, DNSQuestion *Question, char *RR,
                        int RRlen, char *servername, char *Response,
                        int servernamelen);
unsigned char findInStatic(char *name, unsigned int *x);


#endif