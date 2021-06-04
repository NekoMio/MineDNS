#include "net.h"

#include "log.h"

void init_socket() {
#ifdef WIN32
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  // printf ("error\n");
  Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // printf("%s : %d\n", Host, PORT);
  memset(&inAddr, 0, sizeof(inAddr));
  inAddr.sin_family = AF_INET;
  inAddr.sin_addr.s_addr = inet_addr(Host);
  inAddr.sin_port = htons(PORT);
  // printf ("error\n");
  // serverAddr.sin_family = AF_INET;
  // serverAddr.sin_addr.s_addr = inet_addr(PUBLIC_DNS_IP);
  // serverAddr.sin_port = htons(PORT);

  const char REUSE = 1;
  setsockopt(Sock, SOL_SOCKET, SO_REUSEADDR, &REUSE, sizeof(REUSE));
  // printf ("error\n");

  if (bind(Sock, (const SOCKADDR *)&inAddr, sizeof(inAddr))) {
    // printf ("error");
    LOG(ERRORMSG, "Could not bind: %s\n", strerror(errno));
    exit(-1);
  }
  // printf ("error\n");
  
  LOG(HELLOMSG, "Listening on %s:%u\n", Host, PORT);
  LOG(HELLOMSG, "Remote Server: %s\n", PUBLIC_DNS_IP);

  // printf ("error\n");
}
char *recvMessage(unsigned int *ip, int *len, unsigned short *port) {
  char buf[4096];
  int inAddrlen = sizeof(inAddr);
  // inAddr.sin_family = AF_INET;
  // inAddr.sin_addr.S_un.S_addr = inet_addr(Host);
  // inAddr.sin_port = htons(PORT);
  *len = recvfrom(Sock, buf, sizeof(buf), 0, (SOCKADDR *)&inAddr, &inAddrlen);
  // LOG(DEBUGMSG, "%d %s\n", *len, buf);
  if (*len > 0) {
    LOG(NORMALMSG, "Recv Package from %15s:%-5d\n", inet_ntoa(inAddr.sin_addr),
        ntohs(inAddr.sin_port));
    // LOG(LOGMSG, "%d-%02d-%02d %02d:%02d:%02d Recv %15s:%-5d\n", (1900 +
    // p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min,
    // p->tm_sec, inet_ntoa(inAddr.sin_addr), ntohs(inAddr.sin_port));
    *ip = inAddr.sin_addr.s_addr;
    *port = ntohs(inAddr.sin_port);
    char *ret = malloc(4096);
    memcpy(ret, buf, *len);
    return ret;
  } else {
    // LOG(NORMALMSG, "Recv Message Error\n");
    return (char *)-1;
    // exit(-1);
  }

  // return strdup(buf);
}

void sendMessage(char *Msg, int len, unsigned int remote, unsigned short port) {
  // int ret = sendto(Sock, buf)
  inAddr.sin_family = AF_INET;
  inAddr.sin_addr.s_addr = remote;
  inAddr.sin_port = htons(port);
  int ret = sendto(Sock, Msg, len, 0, (SOCKADDR *)&inAddr, sizeof(inAddr));
  if (ret > 0) {
    LOG(NORMALMSG, "Send Package to   %15s:%-5d\n", inet_ntoa(inAddr.sin_addr),
        ntohs(inAddr.sin_port));
  }
  // LOG(DEBUGMSG, "Send Package to %15s:%d\n", inet_ntoa(inAddr.sin_addr),
  // ntohs(inAddr.sin_port)); if (DD) {
  //     for (int i = 0; i < len; i++) {
  //         printf ("%c", Msg[i]);
  //     }
  //     printf ("\n");
  // }
  if (ret < 0) {
    time_t t = time(NULL);
    struct tm *p = localtime(&t);
    LOG(NORMALMSG, "Send Package Error to %15s\n", inet_ntoa(inAddr.sin_addr));
  }
}