#include "dns.h"

#include "log.h"
#include "net.h"
#include "tools.h"

// unsigned short solveA(DNSHeader *Header, DNSQuestion *Question,
//                       char *queryqname, char *Response, unsigned int qip,
//                       unsigned short port) {
//   int RRlen = 0;
//   char *RR;
//   if (findInStatic(queryqname, &RR, &RRlen)) {
//     char *servername = encodeName(queryqname);
//     int servernamelen = strlen(servername);
//     Header->flags = htons(SetFlags(ntohs(Header->flags), QR, 1));
//     if (RRlen == 0) Header->flags = htons(SetFlags(ntohs(Header->flags),
//     RCODE, 3)); Header->qdcount = htons(1); Header->ancount = htons(RRlen ==
//     0 ? 0 : 1); unsigned short len = packData(Header, Question, RR, RRlen,
//     servername,
//                                   Response, servernamelen);
//     free(servername);
//     sendMessage(Response, len, qip, port);
//     return 0;
//   }
//   return solveRemote(Header, Question, queryqname, Response, qip, port);
// }

// unsigned short solveAAAA(DNSHeader *Header, DNSQuestion *Question,
//                       char *queryqname, char *Response, unsigned int qip,
//                       unsigned short port) {
//   int RRlen = 0;
//   char *RR;
//   if (findInStatic(queryqname, &RR, &RRlen) && RRlen == 0) {
//     char *servername = encodeName(queryqname);
//     int servernamelen = strlen(servername);
//     Header->flags = htons(SetFlags(ntohs(Header->flags), QR, 1));
//     if (RRlen == 0) Header->flags = htons(SetFlags(ntohs(Header->flags),
//     RCODE, 3)); Header->qdcount = htons(1); Header->ancount = htons(0);
//     unsigned short len = packData(Header, Question, RR, RRlen, servername,
//                                   Response, servernamelen);
//     free(servername);
//     sendMessage(Response, len, qip, port);
//     return 0;
//   }
//   return solveRemote(Header, Question, queryqname, Response, qip, port);
// }

unsigned short solveRemote(DNSHeader *Header, char *Query, char *Response, unsigned int qip,
                           unsigned short port) {
  DNSQuestion queryquestion;
  int querylen;
  char *queryqname =
          decodeQuestion(Query + sizeof(DNSHeader), &queryquestion, &querylen);
  char *servername = encodeName(queryqname);
  int servernamelen = strlen(servername);
  
  char *RR;
  char RETRR[1024];
  int RRlen;
  time_t ttl;
  if (searchinCache(queryqname, ntohs(queryquestion.qtype), &RR, &RRlen, &ttl)) {
    LOG(DEBUGMSG, "Get From Cache %s: %d\n", queryqname, ntohs(queryquestion.qtype));
    memcpy(RETRR, RR, RRlen);
    memcpy(RETRR, &Header->ID, 2);
    changeTTL(RETRR, RRlen, ttl);
    sendMessage(RETRR, RRlen, qip, port);
    free(queryqname);
    free(servername);
    return 0;
    // Header->flags = htons(SetFlags(ntohs(Header->flags), QR, 1));
    // Header->qdcount = htons(1);
  }
  Header->ID = createMap(Header->ID, qip, port);
  queryForRemote(Header, &queryquestion, servername, servernamelen, Response);
  // decodeHeader(Response, Header);
  // Header->ID = deleteMap(Header->ID);
  // memcpy(Response, Header, sizeof(DNSHeader));
  free(queryqname);
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

  decodeHeader(Query, &queryheader);

  // int querylen = 0;

  char *RR;
  if (unPackFlags(queryheader.flags, QR) == 0) {
    if (unPackFlags(queryheader.flags, OPCODE) == 0) {
      solveRemote(&queryheader, Query, Response, qip, port);
    }
  } else {
    unsigned int ip;
    unsigned short port;
    int tmpid = deleteMap(queryheader.ID, &ip, &port);
    // if (tmpid == -1) {
    //   free(queryqname);
    //   return 0;
    // } else {
    queryheader.ID = tmpid;
    // }
    memcpy(Query, &queryheader, sizeof(queryheader));
    sendMessage(Query, len, ip, port);
    DNSQuestion queryquestion;
    int querylen = 0, RRlen = 0;
    char *queryqname =
        decodeQuestion(Query + sizeof(queryheader), &queryquestion, &querylen);
    unsigned int ttl = unpackforttl(Query + sizeof(queryheader) + querylen,
                                    ntohs(queryheader.qdcount));
    time_t tpnum = 0;
    if (ttl != ~0 && !searchinCache(queryqname, ntohs(queryquestion.qtype),
                                    &RR, &RRlen, &tpnum)) {
      addtoCache(queryqname, ntohs(queryquestion.qtype), ttl + time(NULL),
                 Query, &len);
      free(queryqname);
    }
  }
  // free(queryqname);
  // free(queryheader);
  // free(queryquestion);
}