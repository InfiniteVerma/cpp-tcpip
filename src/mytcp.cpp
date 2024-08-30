#include "mytcp.h"

#include <iostream>
#include <thread>

using namespace std;

MyTcp* MyTcp::myTCPInstance = nullptr;
thread MyTcp::myThread;

MyTcp::MyTcp() { myThread = thread(startTCPThread); }

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
    cout << __FUNCTION__ << " BEGIN (TODO)\n";
    std::this_thread::sleep_for(2000ms);
}

void MyTcp::processTimeouts() {
    cout << __FUNCTION__ << " BEGIN (TODO)\n";
    std::this_thread::sleep_for(2000ms);
}

void MyTcp::recvPackets() {
    cout << __FUNCTION__ << " BEGIN (TODO)\n";
    std::this_thread::sleep_for(2000ms);
}
