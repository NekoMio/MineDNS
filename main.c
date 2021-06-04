#include "main.h"

// unsigned int PORT;
// char Host[16];
// char PUBLIC_DNS_IP[16];

FILE *hosts;

void init_datastruct() {
  cacheData = create_trie();
  DNSHeader Header;
  DNSQuestion Question;
  Header.flags = htons(SetFlags(ntohs(Header.flags), QR, 1));
  // if (RRlen == 0) Header.flags = htons(SetFlags(ntohs(Header.flags), RCODE,
  // 3));
  Header.qdcount = htons(1);
  // Header.ancount = htons(0);
  Question.qtype = htons(ARR);
  Question.qclass = htons(1);
  char str[4097];
  str[4095] = 0;
  int line = 0;
  char sip[4097], name[4097];
  char RRdata[4096], Ansdata[4096];
  unsigned ip;
  while (fgets(str, 4097, hosts)) {
    line++;
    if (str[4095] != 0) {
      LOG(WARNINGMSG, "line %d is too long, ignored\n", line);
      continue;
    }
    sscanf(str, "%s%s", sip, name);
    ip = inet_addr(sip);
    if (ip == INADDR_NONE) {
      LOG(WARNINGMSG, "line %d is invaild, ignored\n", line);
      continue;
    }
    int namelen = strlen(name);
    for (int i = 0; i < namelen; i++) name[i] = tolower(name[i]);
    char *RR = RRdata;
    char *servername = encodeName(name);
    int servernamelen = strlen(servername), RRlen = 0;
    if (ip != 0) {
      Header.ancount = htons(1);
      DNSrr *RRa = malloc(sizeof(DNSrr));
      RRa->type = htons(ARR);
      RRa->dclass = htons(1);
      RRa->ttl = htons(120);
      RRa->rdlengtn = htons(4);
      unsigned short tmp = 0x0cc0;
      memcpy(RR, &tmp, 2);
      // memcpy(RR, servername, servernamelen + 1);
      RR += 2;
      // RR += (servernamelen + 1);
      memcpy(RR, RRa, sizeof(DNSrr));
      RR += sizeof(DNSrr);
      memcpy(RR, &ip, sizeof(int));
      RR += 4;
      RRlen = RR - RRdata;
      RRlen = packData(&Header, &Question, RRdata, RRlen, servername, Ansdata,
                       servernamelen);
      free(RRa);
    } else {
      Header.flags = htons(SetFlags(ntohs(Header.flags), RCODE, 3));
      Header.ancount = 0;
      Question.qtype = htons(AAAARR);
      RRlen = packData(&Header, &Question, NULL, 0, servername, Ansdata,
                       servernamelen);
      insert_trie(cacheData, name, AAAARR, Ansdata, RRlen, -1, NULL);
      Question.qtype = htons(ARR);
      RRlen = packData(&Header, &Question, NULL, 0, servername, Ansdata,
                       servernamelen);
    }
    insert_trie(cacheData, name, ARR, RRdata, RRlen, -1, NULL);
  }
  lru = create_link();
  timeout = create_tree();
}

#define hostsfilenamelen 200
char dnsrelay[hostsfilenamelen + 1] = "dnsrelay.txt";

void parsing_arg(int argc, char const *argv[]) {
  int o, exitval = -1, tmp;
  const char *optstring = "dhl:f:b:p:s:q";
  while ((o = getopt(argc, (char *const *)argv, optstring)) != EOF) {
    switch (o) {
      case 'd':
        DD = 1;
        LOG(NORMALMSG, "DEBUG mode on.\n");
        break;
      case 'q':
        QUIET = 1;
        break;
      case 'l':
        logfile = fopen(optarg, "a");
        if (logfile == NULL) {
          LOG(ERRORMSG, "Unable log to %s", optarg);
          exit(-1);
        }
        LOG(NORMALMSG, "LOG mode on.\n");
        break;
      case 'f':
        if (strlen(optarg) > hostsfilenamelen) goto help;
        strcpy(dnsrelay, optarg);
        break;
      case 'p':
        tmp = atoi(optarg);
        if (tmp == 0) goto help;
        PORT = tmp;
        break;
      case 'b':
        if (strlen(optarg) > 16) goto help;
        strcpy(Host, optarg);
        break;
      case 's':
        if (strlen(optarg) > 16) goto help;
        strcpy(PUBLIC_DNS_IP, optarg);
        break;
      
        
      case 'h':
        exitval = 0;
      help:;
      default:
        printf(
            "Usage:  %s [-d] [-h] [-l <log-file>] [-b <listen>] [-p <port>] "
            "[-f <db-file>] [-s <dns-server>]\n",
            argv[0]);
        printf("Where:  -d                     print debug information\n");
        printf("        -h                     print help\n");
        printf("        -l log-file            print log\n");
        printf("        -f db-file             specify DNS table file\n");
        printf("        -b listen              specify listen ip\n");
        printf("        -p port                specify port number\n");
        printf("        -s dns-server          specify DNS server\n");
        // exit(exitval);
        // break;
        exit(exitval);
    }
  }
}

void hello_msg(int argc, char const *argv[]) {
  LOG(HELLOMSG, "     ___  ___   _   __   _   _____   _____   __   _   _____ \n"); 
  LOG(HELLOMSG, "    /   |/   | | | |  \\ | | | ____| |  _  \\ |  \\ | | /  ___/\n"); 
  LOG(HELLOMSG, "   / /|   /| | | | |   \\| | | |__   | | | | |   \\| | | |___ \n"); 
  LOG(HELLOMSG, "  / / |__/ | | | | | |\\   | |  __|  | | | | | |\\   | \\___  \\\n"); 
  LOG(HELLOMSG, " / /       | | | | | | \\  | | |___  | |_| | | | \\  |  ___| |\n"); 
  LOG(HELLOMSG, "/_/        |_| |_| |_|  \\_| |_____| |_____/ |_|  \\_| /_____/\n");
  LOG(HELLOMSG, "Designed by Wang Xiaohui\n");
  LOG(HELLOMSG, "Yet Another DNS Server\nUse %s -h to get help\n\n", argv[0]);
}
int main(int argc, char const *argv[]) {
  PORT = 53;
  memcpy(Host, "0.0.0.0", sizeof("0.0.0.0"));
  memcpy(PUBLIC_DNS_IP, "10.3.9.44", sizeof("10.3.9.44"));

  parsing_arg(argc, argv);
  // printf ("%s\n", Host);
  hello_msg(argc, argv);
  LOG(DEBUGMSG, "Read Listening %s:%d\n", Host, PORT);
  LOG(DEBUGMSG, "Hosts file in %s\n", dnsrelay);
  if (logfile != NULL) LOG(DEBUGMSG, "Enable log to file\n");
  hosts = fopen(dnsrelay, "r");
  init_socket();
  init_datastruct();
  char *Response = malloc(4096);
  while (1) {
    unsigned int ip;
    int len;
    unsigned short port;
    char *Query = recvMessage(&ip, &len, &port);
    if (Query == (char *)-1) continue;
    // LOG(DEBUGMSG, "Recv Msg\n");
    getResforReq(Query, Response, ip, port, len);
    // sendMessage(Response, len, );
    free(Query);
  }
  free(Response);
  return 0;
}