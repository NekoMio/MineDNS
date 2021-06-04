#include "tools.h"

#include <string.h>

#include "log.h"
#include "tree.h"
#include "trie.h"

int DD = 0, QUIET;
FILE *logfile = NULL;
int cachesize = 0, cachemax = 4096;
LINK *lru;
Map *timeout;

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
char *decodeQuestion(char *Query, DNSQuestion *queryquestion, int *len) {
  // decode doamain name
  char domain[512];
  char *Begin = Query;
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
  *len = (Query - Begin) + sizeof(DNSQuestion);
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

void checkttltimeout() {
  time_t now = time(NULL);
  delete_tree(timeout, now);
  // Node *rt = search_min(timeout);
  // while (rt && rt->key < now) {
  //   LOG(DEBUGMSG, "%s type:%d TTL timeout delete from cache\n", rt->value,
  //   rt->type);

  //   deletenode(timeout, rt);
  //   rt = search_min(timeout);
  // }
}


void lruDelete() {
  link_popback(lru);
}

Node *addttltimeout(char *name, unsigned short type, time_t ttl) {
  return insert_tree(timeout, ttl, name, type);
}

unsigned char searchinCache(char *name, unsigned short type, char **RR,
                            int *len, time_t *ttl) {
  // int len = 0;
  checkttltimeout();
  // updatelru(name, type);
  *RR = search_trie(cacheData, type, name, len, ttl);
  if (*len == -1)
    return 0;
  else
    return 1;
  // return 0;
}

void addtoCache(char *name, unsigned short type, time_t ttl, char *RR,
                int *len) {
  LOG(DEBUGMSG, "%s type:%d Add to Cache\n", name, type);
  if (cachesize == cachemax) {
    lruDelete();
    cachesize--;
  }
  // updatelru(name, type);
  // cachesize++;
  insert_trie(cacheData, name, type, RR, *len, time(NULL), addttltimeout(name, type, ttl));
}

#ifdef __linux__
unsigned int min(unsigned int a, unsigned int b) {
  return a > b ? a : b;
}
#endif

unsigned int unpackforttl(char *Query, unsigned short num) {
  unsigned int minval = ~0;
  DNSrr TMP;
  for (int i = 0; i < num; i++) {
    Query += 2;
    memcpy(&TMP, Query, sizeof(TMP));
    minval = min(minval, ntohl(TMP.ttl));
    Query += ntohs(TMP.rdlengtn);
  }
  return minval;
}

void Change(char *Query, unsigned short num, time_t ttl) {
  DNSrr TMP;
  for (int i = 0; i < num; i++) {
    Query += 2;
    memcpy(&TMP, Query, sizeof(TMP));
    if (ttl == -1)
      TMP.ttl = ntohl(600);
    else
      TMP.ttl = ntohl(TMP.ttl) + ttl - time(NULL);
    memcpy(Query, &TMP, sizeof(TMP));
    Query += ntohs(TMP.rdlengtn);
  }
}

void changeTTL(char *RR, int RRlen, time_t ttl) {
  DNSHeader header;
  DNSQuestion question;
  decodeHeader(RR, &header);

  int querylen = 0;
  decodeQuestion(RR + sizeof(header), &question, &querylen);
  Change(RR + sizeof(header) + querylen, ntohs(header.qdcount), ttl);
}

// unsigned char Used[65536];

unsigned short createMap(unsigned short x, unsigned int ip,
                         unsigned short port) {
  while (IDMapData[IDMapID].used != 0 && time(NULL) - IDMapData[IDMapID].inTime < 2) {
    IDMapID++;
  }
  IDMapData[IDMapID].used = 1;
  IDMapData[IDMapID].ip = ip;
  IDMapData[IDMapID].ID = x;
  IDMapData[IDMapID].port = port;
  IDMapData[IDMapID].inTime = time(NULL);
  return IDMapID++;
}

int deleteMap(unsigned short x, unsigned int *ip, unsigned short *port) {
  IDMapData[x].used = 0;
  *ip = IDMapData[x].ip;
  *port = IDMapData[x].port;
  return IDMapData[x].ID;
}