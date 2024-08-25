#ifndef __RFC793_SOCKET__
#define __RFC793_SOCKET__

#include "tcb.h"

class Socket {
public:
    Socket(int p);

    void debugPrint();

    void bind();
    void listen();
private:
    int socketFd;
    TCB tcb;
};

#endif
