#ifndef __RFC793_MYTCP__
#define __RFC793_MYTCP__

#include <bitset>
#include <condition_variable>
#include <thread>

#include "socket.h"

#define USER_PACKET_COUNT_IN_HEAP 4

class MyTcp {
   public:
    static void createMyTCP(string threadName);

    const static int getMsgQueueID();
    static UINT8 getFD();
    static int getRetval();
    static void setRetVal(int);

    static void waitForThreadToDie();
    static pair<int, int> waitForMessageInBuffer();
    static int insertPacketInSendBuffer(const void* buffer, int size);
    static int getPacketFromBuffer(void* buffer, int slotIdx, int size);

   private:
    MyTcp();
    ~MyTcp();

    static void startTCPThread();
    static void stopTCPThread();

    static MyTcp* myTCPInstance;
    static std::thread myThread;

    static void reactToUserCalls();
    static void processTimeouts();
    static void recvSegment();

    static int getFreeFD();

    static int msgQueueID;
    static bool isStopped;

    // Socket mySocket; // TODO scale this
    static vector<pair<UINT8, Socket*>> mySockets;
    static bool socketsAvailable;

    static std::condition_variable myCV;
    static std::mutex myMutex;

    static bool isFDBusy;
    static bool isRetValAvailable;
    static int retVal;

    static bool isNewPktInBuffer;
    static int slotIdx;

    static uint32_t max_syn_retry_count;

    /*
     * User data
     */
    static char* userPacketData;  // TODO have two buffers
    static bitset<USER_PACKET_COUNT_IN_HEAP> userPacketBitset;
    static std::mutex userPacketMutex;
    static int packetSize;
};

#endif
