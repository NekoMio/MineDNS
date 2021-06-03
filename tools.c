#include "tools.h"
#include "trie.h"
#include <string.h>

int DD = 0;
FILE *logfile = NULL;

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

unsigned short SetFlags(unsigned short flags, int type, unsigned char val) {
  switch (type) {
    case QR:
      return ((flags & QR_MASK) | (val << 15));
    case OPCODE:
      return ((flags & OPCODE_MASK) | (val << 11));
    case AA:
      return ((flags & AA_MASK) | (val << 10));
    case TC:
      return ((flags & TC_MASK) | (val << 9));
    case RD:
      return ((flags & RD_MASK) | (val << 8));
    case RA:
      return ((flags & RA_MASK) | (val << 7));
    case Z:
      return ((flags & Z_MASK) | (val << 4));
    case RCODE:
      return ((flags & RCODE_MASK) | (val));
  }
}

// split header from query
unsigned short decodeHeader(char *Query, DNSHeader *queryheader) {
  memcpy(queryheader, Query, sizeof(DNSHeader));
  return sizeof(DNSHeader);
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
    } else if (c < 45) {
      domain[i] = '.';
    } else {
      domain[i] = c;
    }
  }
  memcpy(queryquestion, Query, sizeof(DNSQuestion));
  return strdup(domain + 1);
}

unsigned short packData(DNSHeader *Header, DNSQuestion *Question, char *RR,
                        int RRlen, char *servername, char *Response,
                        int servernamelen) {
  char *Begin = Response;
  memcpy(Response, Header, sizeof(DNSHeader));
  Response += sizeof(DNSHeader);
  memcpy(Response, servername, servernamelen + 1);
  Response += (servernamelen + 1);
  memcpy(Response, Question, sizeof(DNSQuestion));
  Response += sizeof(DNSQuestion);
  memcpy(Response, RR, RRlen);
  Response += RRlen;
  if (Response - Begin > 512) {
    Response = Begin + 512;
    Header->flags = SetFlags(Header->flags, TC, 1);
    memcpy(Begin, Header, sizeof(DNSHeader));
    return 512;
  }
  return Response - Begin;
}

void push(int x) {
  IDMapQueue[IDMapQueueRight] = x;
  IDMapQueueRight++;
  if (IDMapQueueRight == 65537) IDMapQueueRight = 0;
}
int pop() {
  if (IDMapQueueRight == IDMapQueueLeft) return - 1;
  else return IDMapQueue[IDMapQueueLeft++];
}
unsigned char findInStatic(char *name, char **RR, int *len) {
  // int len = 0;
  *RR = search_trie(cacheData, name, len);
  if (*len == -1) return 0;
  else return 1;
  // return 0;
}

unsigned char Used[65536];

unsigned short createMap(unsigned short x, unsigned int ip, unsigned short port) {
  unsigned short id = pop();
  Used[id] = 1;
  IDMapData[id] = (IDMap){ip, x, port};
  return id;
}

int deleteMap(unsigned short x, unsigned int *ip, unsigned short *port) {
  if (Used[x] == 0) return -1;
  push(x);
  *ip = IDMapData[x].ip;
  *port = IDMapData[x].port;
  Used[x] = 0;
  return IDMapData[x].ID;
}