#include "dns.h"

unsigned short solveA(DNSHeader *Header, DNSQuestion *Question,
                      char *queryqname, char *Response, unsigned int qip,
                      unsigned short port) {
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
    sendMessage(Response, len, qip, port);
    return 0;
  }
  return solveRemote(Header, Question, queryqname, Response, qip, port);
}

unsigned short solveAAAA(char *queryqname, char *Response) {}

unsigned short solveRemote(DNSHeader *Header, DNSQuestion *Question,
                           char *queryname, char *Response, unsigned int qip,
                           unsigned short port) {
  char *servername = encodeName(queryname);
  int servernamelen = strlen(servername);
  Header->ID = createMap(Header->ID, qip, port);
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
  // LOG(8, "%s\n", PUBLIC_DNS_IP);
  sendMessage(Query, len, inet_addr(PUBLIC_DNS_IP), 53);
  return 0;
}

unsigned short getResforReq(char *Query, char *Response, unsigned int qip,
                            unsigned short port, unsigned int len) {
  DNSHeader queryheader;
  DNSQuestion queryquestion;

  decodeHeader(Query, &queryheader);

  char *queryqname = decodeQuestion(Query + sizeof(queryheader), &queryquestion);
  
  if (unPackFlags(queryheader.flags, QR) == 0) {
    if (unPackFlags(queryheader.flags, OPCODE) == 0) {
      switch (ntohs(queryquestion.qtype)) {
        case ARR:
          solveA(&queryheader, &queryquestion, queryqname, Response, qip, port);
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
          solveRemote(&queryheader, &queryquestion, queryqname, Response, qip,
                      port);
      }
    }
  } else {
    unsigned int ip;
    unsigned short port;
    queryheader.ID = deleteMap(queryheader.ID, &ip, &port);
    memcpy(Query, &queryheader, sizeof(queryheader));
    sendMessage(Query, len, ip, port);
  }
  free(queryqname);
  // free(queryheader);
  // free(queryquestion);
}