#include "mytcp.h"
#include <sys/ipc.h>
#include <sys/msg.h>

#include <iostream>
#include <thread>
#include "messages.h"

using namespace std;

MyTcp* MyTcp::myTCPInstance = nullptr;
thread MyTcp::myThread;
int MyTcp::msgQueueID = -1;
bool MyTcp::socketsAvailable = true;
vector<Socket> MyTcp::mySockets;

MyTcp::MyTcp() { 
    // create the msgqueue
    msgQueueID = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    myThread = thread(startTCPThread); 
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
    cout << __FUNCTION__ << " BEGIN (ONGOING)\n";

    MyMsg myMsg;

    int ret = msgrcv(msgQueueID, &myMsg, sizeof(myMsg), 0, IPC_NOWAIT);

    if(ret == -1) return;

    cout << "MyTcp got a message!\n";
    myMsg.print();

    switch(myMsg.mtype)
    {
        case CREATE_SOCKET:
            cout << "Creating a new socket\n";
            if(socketsAvailable)
            {
                Socket newSocket(myMsg.socketName, myMsg.sourceIpAddr, myMsg.port);
                newSocket.setDestIp(myMsg.destIpAddr);
                mySockets.push_back(newSocket);
                cout << "Socket created and added to mySockets\n";
            }
            else
            {
                assert(0);
            }
            break;
        case CLOSE_SOCKET:
            break;
        default:
            assert(0);
    }
}

void MyTcp::processTimeouts() {
    cout << __FUNCTION__ << " BEGIN (TODO)\n";
    std::this_thread::sleep_for(2000ms);
}

void MyTcp::recvPackets() {
    cout << __FUNCTION__ << " BEGIN (TODO)\n";
    std::this_thread::sleep_for(2000ms);
}

const int MyTcp::getMsgQueueID()
{
    return msgQueueID;
}
