#include "main.h"

void init_datastruct() {
    for (int i = 0; i <= 65535; i++) {
        IDMapQueue[i] = i;
    }
    IDMapQueueLeft = 0, IDMapQueueRight = 65535;
}

int main(int argc, char const *argv[])
{
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
        int ip, len;
        char *Query = recvMessage(&ip, &len);
        LOG(DEBUGMSG, "Recv Msg\n");
        char *Response = malloc(4096);
        getResforReq(Query, Response, ip, len);
        // sendMessage(Response, len, );
    }

    return 0;
}