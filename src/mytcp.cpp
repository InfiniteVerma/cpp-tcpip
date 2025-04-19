#include "mytcp.h"

#include <sys/ipc.h>
#include <sys/msg.h>

#include <condition_variable>
#include <mutex>
#include <thread>

#include "common.h"
#include "messages.h"
#include "state_machine.h"
#include "timer.h"

using namespace std;

MyTcp* MyTcp::myTCPInstance = nullptr;
thread MyTcp::myThread;
int MyTcp::msgQueueID = -1;
bool MyTcp::socketsAvailable = true;
vector<pair<UINT8, Socket*>> MyTcp::mySockets;

std::condition_variable MyTcp::myCV;
std::mutex MyTcp::myMutex;

bool MyTcp::isFDBusy = false;

bool MyTcp::isRetValAvailable = false;
int MyTcp::retVal = 0;

bool MyTcp::isNewPktInBuffer = false;
int MyTcp::slotIdx = 0;

bool MyTcp::isStopped = false;

/* user data */
std::mutex MyTcp::userPacketMutex;
char* MyTcp::userPacketData = nullptr;
int MyTcp::packetSize = 0;
bitset<USER_PACKET_COUNT_IN_HEAP> MyTcp::userPacketBitset = {0};

MyTcp::MyTcp() {
    // create the msgqueue
    msgQueueID = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    myThread = thread(startTCPThread);

    userPacketData = new char[1024 * USER_PACKET_COUNT_IN_HEAP];

    // just for testing
    // Timer* timerInstance = Timer::getInstance();
    // ScheduledTask* task = new ScheduledTask(2.0, []() { LOG("Example timeout of 2 seconds, remove!"); });
    // timerInstance->addTimer(task);
}

MyTcp::~MyTcp() {
    LOG("Destructor called!");
    delete[] userPacketData;
}

void MyTcp::createMyTCP(string mainThreadName) {
    Utils::addThreadInfo(mainThreadName);
    if (!myTCPInstance) {
        myTCPInstance = new MyTcp();
    }
}

void MyTcp::startTCPThread() {
    Utils::addThreadInfo("Tcp");
    while (!isStopped) {
        /* 1. react to calls from client */
        reactToUserCalls();
        if (isStopped) break;
        /* 2. Recv pkt */
        recvSegment();
        /* 3. Process timeouts */
        processTimeouts();
    }
}

void MyTcp::stopTCPThread() {
    LOG(__FUNCTION__, " BEGIN");
    isStopped = true;

    /* destroy message queue */
    if (msgctl(msgQueueID, IPC_RMID, NULL) == -1) {
        LOG("Failed in deleting message queue!");
    }

    LOG(__FUNCTION__, " Deleted the msg queue!");
    delete myTCPInstance;
}

void MyTcp::reactToUserCalls() {
    // LOG(__FUNCTION__ << " BEGIN (ONGOING)");

    MyMsg myMsg;

    int ret = msgrcv(msgQueueID, &myMsg, sizeof(myMsg), 0, IPC_NOWAIT);

    if (ret == -1) return;

    LOG("MyTcp got a message!");
    // myMsg.print();

    switch (myMsg.mtype) {
        case CREATE_SOCKET:
            if (socketsAvailable) {  // TODO track a running count of currently created sockets
                std::lock_guard lk(myMutex);
                LOG("Creating a new socket");

                Socket* newSocket = new Socket(myMsg.socketName, myMsg.sourceIpAddr, myMsg.port);
                int newFD = getFreeFD();  // TODO hardcoded
                isFDBusy = true;

                newSocket->setDestIp(myMsg.destIpAddr);
                mySockets.push_back(make_pair(newFD, newSocket));

                LOG("Socket created and added to mySockets, fd: ", newFD, "");
                LOG("Notifying cv!");
                myCV.notify_one();
            } else {
                assert(0);
            }
            break;
        case BIND_SOCKET: {
            std::lock_guard lk(myMutex);
            pair<int, Socket*> socketData = mySockets.back();
            if (socketData.first != myMsg.fd) {
                LOG("Invalid fd passed: <", myMsg.fd, ">");
                retVal = 1;
            } else {
                socketData.second->bind();
                retVal = 0;
            }
            isRetValAvailable = true;
            LOG("BIND_SOCKET called done!");
            myCV.notify_one();
        } break;
        case LISTEN_SOCKET: {
            // dummy
        } break;
        case ACCEPT_SOCKET: {
            /*
             * This is block until 3-way handshake is done with a client?
             */
            std::lock_guard lk(myMutex);
            pair<UINT8, Socket*> socketData = mySockets.back();
            LOG("Listening socket with fd: <", myMsg.fd, ">");
            LOG("Stored fd: ", socketData.first, "");
            if (socketData.first != myMsg.fd) {
                LOG("Invalid fd passed: <", myMsg.fd, ">");
                retVal = 1;
            } else {
                socketData.second->listen();
                // retVal = 0;
            }
            // isRetValAvailable = true;
            // myCV.notify_one();
        } break;
        case CLOSE_SOCKET: {
            std::lock_guard lk(myMutex);
            pair<UINT8, Socket*> socketData = mySockets.back();
            LOG("Listening socket with fd: <", myMsg.fd, ">");
            LOG("Stored fd: ", socketData.first, "");
            if (socketData.first != myMsg.fd) {
                LOG("Invalid fd passed: <", myMsg.fd, ">");
                retVal = 1;
            } else {
                socketData.second->close();
                isFDBusy = false;
                mySockets.pop_back();
                retVal = 0;
            }
            isRetValAvailable = true;
            LOG("CLOSE_SOCKET called TODO!");
            myCV.notify_one();
        } break;
        case CONNECT_SOCKET: {
            LOG("CONNECT_SOCKET called ongoing!");
            pair<UINT8, Socket*> socketData = mySockets.back();
            if (socketData.first != myMsg.fd) {
                LOG("Invalid fd passed: <", myMsg.fd, ">");
                retVal = 1;
            } else {
                int retVal = socketData.second->connect();
                LOG(__FUNCTION__, " retVal from SYN pkt: ", retVal);
                if (retVal != 0) {
                    isRetValAvailable = true;
                    myCV.notify_one();
                } else {
                    // TODO add 5 seconds timer which if fails should return error code
                    // Timer* timerInstance = Timer::getInstance();
                    // ScheduledTask* task = new ScheduledTask(10.0, []() {
                    //    LOG("TIMEOUT HIT for HANDSHAKE!!!!");

                    //    MyTcp::setRetVal(1);
                    //});
                    // timerInstance->addTimer(socketData.second->getLastTransmittedSeqNumber(), task);
                    LOG(__FUNCTION__, " connect SYN call passed. Waiting for reply now!");
                }
            }
        } break;
        case STOP_TCP_THREAD: {
            stopTCPThread();
            LOG(__FUNCTION__, " STOP_TCP_THREAD call reached tcp thread!");
        } break;
        case SEND_PACKET: {
            LOG(__FUNCTION__, " SEND_PACKET called hit slot: ", myMsg.slotIndex, " size: ", myMsg.packetSize);
            lock_guard<mutex> lg(userPacketMutex);  // TODO use getPacketFromBuffer
            char* startPtr = userPacketData + myMsg.slotIndex * 1024;
            char* buffer = new char[myMsg.packetSize];

            memcpy(buffer, startPtr, myMsg.packetSize);
            Utils::hexDump(buffer, myMsg.packetSize);
            userPacketBitset.reset(myMsg.slotIndex);

            // TODO send the packet to server
            assert(mySockets.size() != 0);
            pair<UINT8, Socket*> socketData = mySockets.back();
            if (socketData.second->getCurrentState() != ESTABLISHED) {
                LOG(__FUNCTION__, " ERROR not in established state!");
                // TODO handle this error
                assert(0);
            }

            socketData.second->send(buffer, myMsg.packetSize, 0);
            break;
        }
        default:
            assert(0);
    }
}

/*
 * Assumes mutex is acquired. TODO update comment
 */
void MyTcp::setRetVal(int ret) {
    if (!isRetValAvailable) {
        LOG(__FUNCTION__, "setRetVal flag is not false!!");
        return;  // TODO IDK
    }
    retVal = ret;
    isRetValAvailable = true;
    myCV.notify_one();
}

void MyTcp::processTimeouts() {
    Timer* timerInstance = Timer::getInstance();
    timerInstance->runTimeouts();
}

void MyTcp::recvSegment() {  // TODO pg65
    // LOG(__FUNCTION__, " BEGIN ");

    if (mySockets.empty()) return;

    // TODO iterate and check if they are in LISTEN mode?
    pair<UINT8, Socket*> mySocketData = mySockets.back();

    Socket* mySocket = mySocketData.second;

    char* buffer = new char[65535];
    int size = 0;
    int ret = mySocket->receivePacketBlocking(buffer, size, 1);

    if (ret <= 0) return;  // no pkt received

    LOG(__FUNCTION__, " get a packet! Hexdump:");
    Utils::hexDump(buffer, size);

    /*
     * Packet received from the socket. Send it to the FSM.
     */
    ACTION action = mySocket->updateState(buffer, size);
    // LOG(__FUNCTION__, " current State: ", mySocket->getCurrentState());
    if (!action && mySocket->getCurrentState() == ESTABLISHED) {  // TODO why is first of the two checks needed?
        LOG(__FUNCTION__, " ESTABLISHED!");
        /*
         * Comes here in two cases:
         *  - Ack packet: Below setRetVal unlocks the mutex notifiying server thread that handshake has completed
         *  - Data packet: Store this in buffer and notify that packet is available to the server
         */
        MyTcp::setRetVal(0);  // unlocks the mutex and so client thread is notified

        if (size > static_cast<int>(sizeof(IPHeader) + sizeof(TCPHeader))) {
            // save packet to buffer after stripping ip and tcp header
            int slot = MyTcp::insertPacketInSendBuffer(buffer + sizeof(IPHeader) + sizeof(TCPHeader),
                                                       size - sizeof(IPHeader) - sizeof(TCPHeader));
            if (slot == -1) assert(0);
        }
    } else if (!action) {
        LOG(__FUNCTION__, " action is NULL and it's not ESTABLISHED, assuming FSM wants us to discard the packet");
    } else {
        mySocket->executeNextAction(action, buffer, size);
        if (mySocket->getCurrentState() == ESTABLISHED || mySocket->getCurrentState() == SYN_RECEIVED) {
            LOG(__FUNCTION__, " trying to unlock mutex");
            isRetValAvailable = true;
            MyTcp::setRetVal(0);  // unlocks the mutex and so client thread is notified
        }
        LOG(__FUNCTION__, " executed next action");
    }

    delete[] buffer;

    // LOG(__FUNCTION__, " After processing a packet!");
    // mySocket->debugPrint();
}

const int MyTcp::getMsgQueueID() { return msgQueueID; }

int MyTcp::getFreeFD() { return 20; }

UINT8 MyTcp::getFD() {
    std::unique_lock lk(myMutex);
    myCV.wait(lk, [] { return isFDBusy; });  // wakes up if isFDBusy is set
    LOG("Conditional variable notified and isFDBusy is set!");
    UINT8 fd = mySockets.back().first;
    isFDBusy = false;
    myMutex.unlock();
    return fd;
}

int MyTcp::getRetval() {
    std::unique_lock lk(myMutex);
    myCV.wait(lk, [] { return isRetValAvailable; });  // wakes up if the flag is set
    int ret = retVal;
    LOG("Conditional variable notified and isRetValAvailable is set to val: ", retVal);
    isRetValAvailable = false;
    myMutex.unlock();
    return ret;
}

pair<int, int> MyTcp::waitForMessageInBuffer() {
    LOG(__FUNCTION__, " BEGIN");
    std::unique_lock lk(userPacketMutex);
    myCV.wait(lk, [] { return isNewPktInBuffer; });  // wakes up if the flag is set
    LOG(__FUNCTION__, " Conditional variable notified and isNewPktInBuffer is set to val: ", slotIdx);
    isNewPktInBuffer = false;
    int size = packetSize;
    packetSize = 0;
    myMutex.unlock();
    return make_pair(slotIdx, size);
}

int MyTcp::getPacketFromBuffer(void* buffer, int slotIdx, int size) {
    if (slotIdx < 0 || slotIdx > USER_PACKET_COUNT_IN_HEAP) assert(0);

    lock_guard<mutex> lg(userPacketMutex);  // TODO use getPacketFromBuffer
    char* startPtr = userPacketData + slotIdx * 1024;

    memcpy(buffer, startPtr, size);
    Utils::hexDump((const char*)buffer, size);
    userPacketBitset.reset(slotIdx);
    return size;
}

void MyTcp::waitForThreadToDie() { myThread.join(); }

int MyTcp::insertPacketInSendBuffer(const void* buffer, int size) {
    LOG(__FUNCTION__, " BEGIN: ", size);
    lock_guard<mutex> lg(userPacketMutex);
    int slot = -1;
    for (int i = 0; i < USER_PACKET_COUNT_IN_HEAP; i++) {
        if (!userPacketBitset[i]) {
            LOG(__FUNCTION__, " found free slot at index: ", i);
            userPacketBitset.set(i);
            char* startPtr = userPacketData + i * 1024;
            memcpy(startPtr, buffer, size);
            slot = i;
            break;
        }
    }

    packetSize = size;
    isNewPktInBuffer = true;
    LOG(__FUNCTION__, " notifying for pkt in send buffer slot: ", slot);
    myCV.notify_one();
    return slot;
}
