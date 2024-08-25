#ifndef __RFC793_SOCKET__
#define __RFC793_SOCKET__

#include "tcb.h"

class Socket {
public:
    Socket(std::string s);

    void debugPrint();

    /* Public APIs */
    void bind(int p);
    int connect(int p);
    void listen();
    void send(const char* message, size_t len, int flags);
    void close();
    int accept();
private:
    int threeWayHandshakeClient();
    int socketFd;
    TCB tcb;
    std::string desc;
};

#endif
