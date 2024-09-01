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

bool MyTcp::isFDAvailable = false;
bool MyTcp::isRetValAvailable = false;
bool MyTcp::isStopped = false;
int MyTcp::retVal = 0;

MyTcp::MyTcp() {
    // create the msgqueue
    msgQueueID = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    myThread = thread(startTCPThread);

    // just for testing
    // Timer* timerInstance = Timer::getInstance();
    // ScheduledTask* task = new ScheduledTask(2.0, []() { LOG("Example timeout of 2 seconds, remove!"); });
    // timerInstance->addTimer(task);
}

MyTcp::~MyTcp() { LOG("Destructor called!"); }

void MyTcp::createMyTCP(string mainThreadName) {
    Utils::addThreadInfo(mainThreadName);
    if (!myTCPInstance) {
        myTCPInstance = new MyTcp();
    }
}

void MyTcp::startTCPThread() {
    Utils::addThreadInfo("Kernel");
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
            if (socketsAvailable) {
                std::lock_guard lk(myMutex);
                LOG("Creating a new socket");

                Socket* newSocket = new Socket(myMsg.socketName, myMsg.sourceIpAddr, myMsg.port);
                int newFD = getFreeFD();
                isFDAvailable = true;

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
        case CLOSE_SOCKET:
            LOG("CLOSE_SOCKET called TODO!");
            break;
        case CONNECT_SOCKET: {
            LOG("CONNECT_SOCKET called ongoing!");
            pair<UINT8, Socket*> socketData = mySockets.back();
            if (socketData.first != myMsg.fd) {
                LOG("Invalid fd passed: <", myMsg.fd, ">");
                retVal = 1;
            } else {
                int retVal = socketData.second->connect();
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
            LOG(__FUNCTION__, " STOP_TCP_THREAD call reached kernel!");
        } break;
        default:
            assert(0);
    }
}

/*
 * Assumes mutex is acquired. TODO update comment
 */
void MyTcp::setRetVal(int ret) {
    retVal = 1;
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
    if (!action && mySocket->getCurrentState() == ESTABLISHED) {  // TODO why is first of the two checks needed?
        LOG(__FUNCTION__, " ESTABLISHED!");
        MyTcp::setRetVal(0);  // unlocks the mutex and so client thread is notified
    } else if (!action) {
        LOG(__FUNCTION__, " action is NULL, assuming FSM wants us to discard the packet");
    } else {
        mySocket->executeNextAction(action);
        LOG(__FUNCTION__, " executed next action");
    }

#if 0
    if (mySocket->getCurrentState() == LISTEN) {
        LOG(__FUNCTION__, " get a packet! Hexdump:");
        Utils::hexDump(buffer, size);
        LOG(__FUNCTION__, " ack number: ", pkt.getAck());

        if (pkt.isRSTSet()) {
            // discarded
        } else if (pkt.isACKSet() || pkt.isSYNSet()) {
            Timer* timerInstance = Timer::getInstance();
            timerInstance->delTimer(pkt.getAck());  // in case seq doesn't match this fails silently

            ACTION action = mySocket->updateState(buffer, size);
            if (!action && mySocket->getCurrentState() == ESTABLISHED) {  // TODO why is first of the two checks needed?
                LOG(__FUNCTION__, " ESTABLISHED!");
                MyTcp::setRetVal(0);  // unlocks the mutex and so client thread is notified
            } else {
                mySocket->executeNextAction(action);
                LOG(__FUNCTION__, " executed next action");
            }
        } else {
            assert(0);
        }
    } else {
        LOG(__FUNCTION__, " socket not in listen stage and received a pkt!");
        // An incoming segment containing RST is discarded
        // An incoming segment not containing a RST causes a RST to be sent in response
        if (pkt.isRSTSet()) {
            LOG(__FUNCTION__, " rst packet, discarding");
        } else {
            LOG(__FUNCTION__, " not containing rst, replying with rst");
            // TODO
            assert(0);
        }
    }
#endif

    LOG(__FUNCTION__, " After processing a packet!");
    mySocket->debugPrint();

    delete[] buffer;
}

const int MyTcp::getMsgQueueID() { return msgQueueID; }

int MyTcp::getFreeFD() { return 20; }

UINT8 MyTcp::getFD() {
    std::unique_lock lk(myMutex);
    myCV.wait(lk, [] { return isFDAvailable; });  // wakes up if isFDAvailable is set
    LOG("Conditional variable notified and isFDAvailable is set!");
    UINT8 fd = mySockets.back().first;
    isFDAvailable = false;
    myMutex.unlock();
    return fd;
}

int MyTcp::getRetval() {
    std::unique_lock lk(myMutex);
    myCV.wait(lk, [] { return isRetValAvailable; });  // wakes up if the flag is set
    LOG("Conditional variable notified and isRetValAvailable is set to val: ", retVal, "");
    int ret = retVal;
    isRetValAvailable = false;
    myMutex.unlock();
    return ret;
}

void MyTcp::waitForThreadToDie() { myThread.join(); }
