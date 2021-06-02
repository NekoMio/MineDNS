#include "dns.h"

unsigned short solveA(DNSHeader *Header, DNSQuestion *Question,
                      char *queryqname, char *Response, unsigned int qip) {
  unsigned int x = 0;
  int RRlen = 0;
  char *RR = malloc(1024);
  char *RRbegin = RR;
  if (findInStatic(queryqname, &x)) {
    char *servername = encodeName(queryqname);
    int servernamelen = strlen(servername);
    SetFlags(Header->flags, QR, 1);
    Header->qdcount = 1;
    Header->ancount = (x == 0 ? 0 : 1);
    if (x != 0) {
      DNSrr *RRa = malloc(sizeof(DNSrr));
      RRa->type = ARR;
      RRa->dclass = 1;
      RRa->ttl = 600;
      RRa->rdlengtn = 4;
      memcpy(RR, servername, servernamelen);
      RR += servernamelen;
      memcpy(RR, RRa, sizeof(DNSrr));
      RR += sizeof(DNSrr);
      memcpy(RR, &x, sizeof(int));
      RR += 4;
      RRlen = RR - RRbegin;
    }
    unsigned short len = packData(Header, Question, RRbegin, RRlen, servername,
                                  Response, servernamelen);
    free(servername);
    sendMessage(Response, len, qip);
    return 0;
  }
  return solveRemote(Header, Question, queryqname, Response, qip);
}

unsigned short solveAAAA(char *queryqname, char *Response) {}

unsigned short solveRemote(DNSHeader *Header, DNSQuestion *Question,
                           char *queryname, char *Response, unsigned int qip) {
  char *servername = encodeName(queryname);
  int servernamelen = strlen(servername);
  Header->ID = createMap(Header->ID, qip);
  queryForRemote(Header, Question, servername, servernamelen, Response);
  // decodeHeader(Response, Header);
  // Header->ID = deleteMap(Header->ID);
  // memcpy(Response, Header, sizeof(DNSHeader));
  free(servername);
  return 0;
}

unsigned short queryForRemote(DNSHeader *Header, DNSQuestion *Question,
                              char *servername, int servernamelen,
                              char *Response) {
  char *Query = malloc(1024);
  int len =
      packData(Header, Question, NULL, 0, servername, Query, servernamelen);
  sendMessage(Query, len, inet_addr(PUBLIC_DNS_IP));
  return 0;
}

unsigned short getResforReq(char *Query, char *Response, unsigned int qip, unsigned int len) {
  DNSHeader *queryheader = malloc(sizeof(DNSHeader));
  DNSQuestion *queryquestion = malloc(sizeof(DNSQuestion));

  decodeHeader(Query, queryheader);
  char *queryqname = decodeQuestion(Query + sizeof(DNSHeader), queryquestion);
  if (unPackFlags(queryheader->flags, QR) == 0) {
    if (unPackFlags(queryheader->flags, OPCODE) == 0) {
      switch (ntohs(queryquestion->qtype)) {
        case ARR:
          solveA(queryheader, queryquestion, queryqname, Response, qip);
          break;
        case AAAARR:
          // solveAAAA(queryqname, Response);
          // break;
        case CNAMERR:
          // solveCNAME(queryqname, Response);
          // break;
        case NSRR:
        case SOARR:
        case PTRRR:
        case MXRR:
        case TXTRR:
        case SRVRR:
        default:
          solveRemote(queryheader, queryquestion, queryqname, Response, qip);
      }
    }
  } else {
    unsigned int ip;
    queryheader->ID = deleteMap(queryheader->ID, &ip);
    memcpy(Query, queryheader, sizeof(queryheader));
    sendMessage(Query, len, qip);
  }
  free(queryqname);
  free(queryheader);
  free(queryquestion);
}