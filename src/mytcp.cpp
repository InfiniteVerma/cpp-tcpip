#include "mytcp.h"

#include <sys/ipc.h>
#include <sys/msg.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "messages.h"
#include "timer.h"

using namespace std;

MyTcp* MyTcp::myTCPInstance = nullptr;
thread MyTcp::myThread;
int MyTcp::msgQueueID = -1;
bool MyTcp::socketsAvailable = true;
vector<pair<UINT8, Socket>> MyTcp::mySockets;

std::condition_variable MyTcp::myCV;
std::mutex MyTcp::myMutex;

bool MyTcp::isFDAvailable = false;
bool MyTcp::isRetValAvailable = false;
int MyTcp::retVal = 0;

MyTcp::MyTcp() {
    // create the msgqueue
    msgQueueID = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    myThread = thread(startTCPThread);

    // just for testing
    Timer* timerInstance = Timer::getInstance();
    ScheduledTask* task = new ScheduledTask(10.0);
    timerInstance->addTask(task);
}

MyTcp::~MyTcp() {
    cout << "Stopping myThread\n";
    if (myThread.joinable()) {
        myThread.join();  // TODO cleanup
    } else {
        cout << "ERROR, can't join!\n";
    }
}

void MyTcp::createMyTCP() {
    if (!myTCPInstance) {
        myTCPInstance = new MyTcp();
    }
}

void MyTcp::startTCPThread() {
    while (1) {
        /* 1. react to calls from client */
        reactToUserCalls();
        /* 2. Process timeouts */
        processTimeouts();
        /* 3. Recv pkt */
        recvPackets();
    }
}

void MyTcp::reactToUserCalls() {
    // cout << __FUNCTION__ << " BEGIN (ONGOING)\n";

    MyMsg myMsg;

    int ret = msgrcv(msgQueueID, &myMsg, sizeof(myMsg), 0, IPC_NOWAIT);

    if (ret == -1) return;

    cout << "MyTcp got a message!\n";
    myMsg.print();

    switch (myMsg.mtype) {
        case CREATE_SOCKET:
            if (socketsAvailable) {
                std::lock_guard lk(myMutex);
                cout << "Creating a new socket\n";

                Socket newSocket(myMsg.socketName, myMsg.sourceIpAddr, myMsg.port);
                int newFD = getFreeFD();
                isFDAvailable = true;

                newSocket.setDestIp(myMsg.destIpAddr);
                mySockets.push_back(make_pair(newFD, newSocket));

                cout << "Socket created and added to mySockets, fd: " << newFD << "\n";
                cout << "Notifying cv!\n";
                myCV.notify_one();
            } else {
                assert(0);
            }
            break;
        case BIND_SOCKET: {
            std::lock_guard lk(myMutex);
            pair<int, Socket> socketData = mySockets.back();
            if (socketData.first != myMsg.fd) {
                cout << "Invalid fd passed: <" << myMsg.fd << ">\n";
                retVal = 1;
            } else {
                socketData.second.bind();
                retVal = 0;
            }
            isRetValAvailable = true;
            cout << "BIND_SOCKET called done!\n";
            myCV.notify_one();
        } break;
        case LISTEN_SOCKET: {
            /*
             * This is block until 3-way handshake is done with a client?
             */
            std::lock_guard lk(myMutex);
            pair<UINT8, Socket> socketData = mySockets.back();
            cout << "Listening socket with fd: <" << myMsg.fd << ">\n";
            cout << "Stored fd: " << socketData.first << "\n";
            if (socketData.first != myMsg.fd) {
                cout << "Invalid fd passed: <" << myMsg.fd << ">\n";
                retVal = 1;
            } else {
                socketData.second.listen();
                // retVal = 0;
            }
            // isRetValAvailable = true;
            cout << "LISTEN_SOCKET called !, not notifying conditional variable. Will wait till handshake completes or "
                    "a timeout?\n";
            // myCV.notify_one();
        } break;
        case CLOSE_SOCKET:
            cout << "CLOSE_SOCKET called TODO!\n";
            break;
        default:
            assert(0);
    }
}

void MyTcp::processTimeouts() {
    // cout << __FUNCTION__ << " BEGIN (TODO)\n";
    // cout << __FUNCTION__ << " iterating through timer tasks and checking!\n";

    Timer* timerInstance = Timer::getInstance();
    timerInstance->runTimeouts();
}

void MyTcp::recvPackets() {
    // cout << __FUNCTION__ << " BEGIN (TODO)\n";
}

const int MyTcp::getMsgQueueID() { return msgQueueID; }

int MyTcp::getFreeFD() { return 20; }

UINT8 MyTcp::getFD() {
    std::unique_lock lk(myMutex);
    myCV.wait(lk, [] { return isFDAvailable; });  // wakes up if isFDAvailable is set
    cout << "Conditional variable notified and isFDAvailable is set!\n";
    UINT8 fd = mySockets.back().first;
    isFDAvailable = false;
    myMutex.unlock();
    return fd;
}

int MyTcp::getRetval() {
    std::unique_lock lk(myMutex);
    myCV.wait(lk, [] { return isRetValAvailable; });  // wakes up if the flag is set
    cout << "Conditional variable notified and isRetValAvailable is set to val: " << retVal << "\n";
    int ret = retVal;
    isRetValAvailable = false;
    myMutex.unlock();
    return ret;
}
