#ifndef __RFC793_MYTCP__
#define __RFC793_MYTCP__

#include <thread>

class MyTcp {
public:
    static void createMyTCP();
private:
    MyTcp();
    ~MyTcp();

    static void startTCPThread();
    static MyTcp* myTCPInstance;
    static std::thread myThread;

    static void reactToUserCalls();
    static void processTimeouts();
    static void recvPackets();
};

#endif
