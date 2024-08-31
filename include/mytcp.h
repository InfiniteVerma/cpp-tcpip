#ifndef __RFC793_MYTCP__
#define __RFC793_MYTCP__

#include <condition_variable>
#include <thread>
#include "socket.h"

class MyTcp {
public:
    static void createMyTCP();

    const static int getMsgQueueID();
    static int getFD();
private:
    MyTcp();
    ~MyTcp();

    static void startTCPThread();
    static MyTcp* myTCPInstance;
    static std::thread myThread;

    static void reactToUserCalls();
    static void processTimeouts();
    static void recvPackets();

    static int getFreeFD();
    
    static int msgQueueID; 

    //Socket mySocket; // TODO scale this
    static vector<pair<int, Socket>> mySockets;
    static bool socketsAvailable;

    static std::condition_variable myCV;
    static std::mutex myMutex;

    static bool isFDAvailable;
};

#endif
