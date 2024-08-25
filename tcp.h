#ifndef __RFC793_TCP__
#define __RFC793_TCP__

struct TCB {
};

class Socket {
public:
    Socket(int p);

    void debugPrint();
private:
    int port;
};

#endif
