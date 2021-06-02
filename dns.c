#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dns.h"
#include "tools.h"

unsigned short solveA(DNSHeader *Header, DNSQuestion *Question,
                      char *queryqname, char *Response) {
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
    return len;
  }
  return solveRemote(Header, Question, queryqname, Response);
}

unsigned short solveAAAA(char *queryqname, char *Response) {}

unsigned short solveRemote(DNSHeader *Header, DNSQuestion *Question,
                           char *queryname, char *Response) {
  char *servername = encodeName(queryname);
  int servernamelen = strlen(servername);
  Header->ID = createMap(Header->ID);
  queryForRemote(Header, Question, Response);
  decodeHeader(Response, Header);
  Header->ID = deleteMap(Header->ID);
  memcpy(Response, Header, sizeof(DNSHeader));
}

unsigned short getResforReq(char *Query, char *Response) {
  DNSHeader *queryheader = malloc(sizeof(DNSHeader));
  DNSQuestion *queryquestion = malloc(sizeof(DNSQuestion));

  Query += decodeHeader(Query, queryheader);
  char *queryqname = decodeQuestion(Query, queryquestion);

  if (unPackFlags(queryheader->flags, OPCODE) == 0) {
    switch (queryquestion->qtype) {
      case ARR:
        solveA(queryheader, queryquestion, queryqname, Response);
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
        solveRemote(queryheader, queryquestion, queryqname, Response);
    }
  }

  free(queryqname);
  free(queryheader);
  free(queryquestion);
}