#ifndef __RFC793_MYTCP__
#define __RFC793_MYTCP__

#include <condition_variable>
#include <thread>

#include "socket.h"

class MyTcp {
   public:
    static void createMyTCP(string threadName);
    static void stopMyTCP();

    const static int getMsgQueueID();
    static UINT8 getFD();
    static int getRetval();
    static void setRetVal(int);

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
    static bool isStopped;

    // Socket mySocket; // TODO scale this
    static vector<pair<UINT8, Socket>> mySockets;
    static bool socketsAvailable;

    static std::condition_variable myCV;
    static std::mutex myMutex;

    static bool isFDAvailable;
    static bool isRetValAvailable;
    static int retVal;
};

#endif
