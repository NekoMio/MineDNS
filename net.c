#include "net.h"


void init_socket() {
#ifdef WIN32
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    // printf ("error\n");
    Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    memset(&inAddr, 0, sizeof(inAddr));
    inAddr.sin_family = AF_INET;
    inAddr.sin_addr.s_addr = inet_addr(Host);
    inAddr.sin_port = htons(PORT);
    // printf ("error\n");
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_addr.s_addr = inet_addr(PUBLIC_DNS_IP);
    // serverAddr.sin_port = htons(PORT);

    const int REUSE = 1;
    setsockopt(Sock, SOL_SOCKET, SO_REUSEADDR, &REUSE, sizeof(REUSE));
    // printf ("error\n");
    
    if (bind(Sock, (const SOCKADDR *)&inAddr, sizeof(inAddr))) {
        // printf ("error");
        LOG(ERRORMSG, "Could not bind: %s\n", strerror(errno));
        exit(-1);
    }
    // printf ("error\n");
    LOG(NORMALMSG, "Server : %s\n", PUBLIC_DNS_IP);
    LOG(NORMALMSG, "Listening on %s:%u\n", Host, PORT);
    // printf ("error\n");
}
char *recvMessage(unsigned int *ip, int *len) {
    char buf[4096];
    int inAddrlen = sizeof(inAddr);
    inAddr.sin_addr.S_un.S_addr = inet_addr(Host);
    *len = recvfrom(Sock, buf, sizeof(buf), 0, (SOCKADDR *)&inAddr, &inAddrlen);
    // LOG(DEBUGMSG, "%d %s\n", *len, buf);
    if (*len > 0) {
        time_t t = time(NULL);
        struct tm *p = localtime(&t);
        LOG(LOGMSG | DEBUGMSG | NORMALMSG, "%d-%02d-%02d %02d:%02d:%02d Recv %15s:%-5d\n", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, inet_ntoa(inAddr.sin_addr), ntohs(inAddr.sin_port));
    }
    *ip = inAddr.sin_addr.S_un.S_addr;
    char *ret = malloc(4096);
    memcpy(ret, buf, *len);
    return ret;
    // return strdup(buf);
}

void sendMessage(char *Msg, int len, unsigned int remote) {
    // int ret = sendto(Sock, buf)
    inAddr.sin_addr.s_addr = remote;
    inAddr.sin_port = htons(PORT);
    int ret = sendto(Sock, Msg, len, 0, (SOCKADDR *)&inAddr, sizeof(inAddr));
    for (int i = 0; i < len; i++) {
        printf ("%c", Msg[i]);
    }
    printf ("\n");
    if (ret < 0) {
        LOG(WARNINGMSG, "Send Package Error to %s\n", inet_ntoa(inAddr.sin_addr));
    }
}