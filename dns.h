#ifndef dnsH
#define dnsH
#include <stdlib.h>
#include <string.h>
enum { QR, OPCODE, AA, TC, RD, RA, Z, RCODE };

enum {
  ARR = 1,
  NSRR = 2,
  CNAMERR = 5,
  SOARR = 6,
  PTRRR = 12,
  MXRR = 15,
  TXTRR = 16,
  AAAARR = 28,
  SRVRR = 33
};

static const unsigned int QR_MASK = ~0x8000;
static const unsigned int OPCODE_MASK = ~0x7800;
static const unsigned int AA_MASK = ~0x0400;
static const unsigned int TC_MASK = ~0x0200;
static const unsigned int RD_MASK = ~0x0100;
static const unsigned int RA_MASK = ~0x0080;
static const unsigned int Z_MASK = ~0x0070;
static const unsigned int RCODE_MASK = ~0x000F;

typedef struct DNSHEADER {
  unsigned short ID;
  unsigned short flags;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short nscount;
  unsigned short arcount;
} DNSHeader;

typedef struct DNSQUESTION {
  unsigned short qtype;
  unsigned short qclass;
} DNSQuestion;

#pragma pack(push, 1)
typedef struct DNSRR {
  unsigned short type;
  unsigned short dclass;
  unsigned int ttl;
  unsigned short rdlengtn;
} DNSrr;
#pragma pack(pop)

extern unsigned int PORT;
extern char Host[16];
extern char PUBLIC_DNS_IP[16];

unsigned short solveA(DNSHeader *Header, DNSQuestion *Question,
                      char *queryqname, char *Response, unsigned int qip);
unsigned short solveAAAA(char *queryqname, char *Response);
unsigned short solveRemote(DNSHeader *Header, DNSQuestion *Question,
                           char *queryname, char *Response, unsigned int qip);

unsigned short getResforReq(char *Query, char *Response, unsigned int qip,
                            unsigned int len);
unsigned short queryForRemote(DNSHeader *Header, DNSQuestion *Question,
                              char *servername, int servernamelen,
                              char *Response);

#endif