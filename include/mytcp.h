#ifndef __RFC793_MYTCP__
#define __RFC793_MYTCP__

#include <thread>
#include "socket.h"

class MyTcp {
public:
    static void createMyTCP();

    const static int getMsgQueueID();
private:
    MyTcp();
    ~MyTcp();

    static void startTCPThread();
    static MyTcp* myTCPInstance;
    static std::thread myThread;

    static void reactToUserCalls();
    static void processTimeouts();
    static void recvPackets();
    
    static int msgQueueID; 

    //Socket mySocket; // TODO scale this
    static vector<Socket> mySockets;
    static bool socketsAvailable;
};

#endif
