#include "tools.h"
#include <string.h>

int DD = 1;
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
unsigned char findInStatic(char *name, unsigned int *x) {
  return 0;
}

unsigned short createMap(unsigned short x, unsigned int ip, unsigned short port) {
  unsigned short id = pop();
  IDMapData[id] = (IDMap){ip, x, port};
  return id;
}

unsigned short deleteMap(unsigned short x, unsigned int *ip, unsigned short *port) {
  push(x);
  *ip = IDMapData[x].ip;
  *port = IDMapData[x].port;
  return IDMapData[x].ID;
}

void LOG(int type, char *format, ...) {
  // printf ("%d\n", type);
  va_list args;
  va_start (args, format);
  // char *format = va_arg(args, char*);
  if ((type & NORMALMSG)) {
    vprintf (format, args);
    return;
  }
  if ((type & WARNINGMSG)) {
    printf ("WARNING: ");
    vprintf (format, args);
    return;
  }
  if ((type & ERRORMSG)) {
    printf ("ERROR: ");
    vprintf (format, args);
    return;
  }
  if ((type & DEBUGMSG) && DD) {
    printf ("DEBUG: ");
    vprintf (format, args);
    return;
  }
  if ((type & LOGMSG) && logfile) {
    vfprintf(logfile, format, args);
    return;
  }
  va_end(args);
}