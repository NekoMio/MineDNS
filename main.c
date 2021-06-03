#include "main.h"


// unsigned int PORT;
// char Host[16];
// char PUBLIC_DNS_IP[16];

void init_datastruct() {
    for (int i = 0; i <= 65535; i++) {
        IDMapQueue[i] = i;
    }
    IDMapQueueLeft = 0, IDMapQueueRight = 65535;
}

int main(int argc, char const *argv[])
{
    PORT = 53;
    memcpy (Host, "0.0.0.0", sizeof("0.0.0.0"));
    // printf ("%s\n", Host);
    memcpy (PUBLIC_DNS_IP, "119.29.29.29", sizeof("119.29.29.29"));
    LOG(DEBUGMSG, "Read Listening %s:%d\n", Host, PORT);
    init_socket();
    init_datastruct();

#ifdef WIN32
    // int nonBlock = 1;
    // ioctlsocket(Sock, FIONBIO, &nonBlock);
#endif

#ifdef __LINUX__
    ioctl(Sock, FIONBIO, 1);
#endif

    while (1) {
        unsigned int ip;
        int len;
        unsigned short port;
        char *Query = recvMessage(&ip, &len, &port);
        // LOG(DEBUGMSG, "Recv Msg\n");
        char *Response = malloc(4096);
        getResforReq(Query, Response, ip, port, len);
        // sendMessage(Response, len, );
    }

    return 0;
}