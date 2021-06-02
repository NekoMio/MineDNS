#include "tools.h"
#include <string.h>

unsigned short unPackFlags(unsigned short flags, int type) {
  switch (type) {
    case QR:
      return flags >> 15;
    case OPCODE:
      return (flags >> 11) & 0xf;
    case AA:
      return (flags >> 10) & 0x1;
    case TC:
      return (flags >> 9) & 0x1;
    case RD:
      return (flags >> 8) & 0x1;
    case RA:
      return (flags >> 7) & 0x1;
    case Z:
      return (flags >> 4) & 0x7;
    case RCODE:
      return flags & 0xf;
  }
}

void SetFlags(unsigned short flags, int type, unsigned char val) {
  switch (type) {
    case QR:
      flags = (flags & QR_MASK) | (val << 15);
    case OPCODE:
      flags = (flags & OPCODE_MASK) | (val << 11);
    case AA:
      flags = (flags & AA_MASK) | (val << 10);
    case TC:
      flags = (flags & TC_MASK) | (val << 9);
    case RD:
      flags = (flags & RD_MASK) | (val << 8);
    case RA:
      flags = (flags & RA_MASK) | (val << 7);
    case Z:
      flags = (flags & Z_MASK) | (val << 4);
    case RCODE:
      flags = (flags & RCODE_MASK) | (val);
  }
}

// split header from query
unsigned short decodeHeader(char *Query, DNSHeader *queryheader) {
  memcpy(queryheader, Query, sizeof(queryheader));
  return sizeof(queryheader);
}

char *encodeName(char *name) {
  char domain[512];
  const char *begin = name;
  const char *pos;
  int len = 0, i = 0;
  while ((pos = strchr(begin, '.'))) {
    len = pos - begin;
    domain[i] = len;
    i += 1;
    memcpy(domain + i, begin, len);
    i += len;

    begin = pos + 1;
  }
  len = strlen(name) - (begin - name);

  domain[i] = len;
  i += 1;
  memcpy(domain + i, begin, len);
  i += len;
  domain[i] = 0;
  return strdup(domain);
}

// split domain and question from query
char *decodeQuestion(char *Query, DNSQuestion *queryquestion) {
  // decode doamain name
  char domain[512];
  for (int i = 0; i < 512; i++) {
    char c = *Query;
    Query++;
    if (c == 0) {
      domain[i] = 0;
      break;
    } else if (c <= 63) {
      domain[i] = '.';
    } else {
      domain[i - 1] = c;
    }
  }
  memcpy(queryquestion, Query, sizeof(queryquestion));
  return strdup(domain);
}

unsigned short packData(DNSHeader *Header, DNSQuestion *Question, char *RR,
                        int RRlen, char *servername, char *Response,
                        int servernamelen) {
  char *Begin = Response;
  memcpy(Response, Header, sizeof(DNSHeader));
  Response += sizeof(DNSHeader);
  memcpy(Response, servername, servernamelen);
  Response += servernamelen;
  memcpy(Response, Question, sizeof(DNSQuestion));
  Response += sizeof(DNSQuestion);
  memcpy(Response, RR, RRlen);
  Response += RRlen;
  if (Response - Begin > 512) {
    Response = Begin + 512;
    SetFlags(Header->flags, TC, 1);
    memcpy(Begin, Header, sizeof(DNSHeader));
    return 512;
  }
  return Response - Begin;
}

unsigned char findInStatic(char *name, unsigned int *x) {

}

unsigned short createMap(unsigned short x, int ip) {

}

unsigned short deleteMap(unsigned short x, int *ip) {

}