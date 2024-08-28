#include "tcb.h"

#include "state_machine.h"

#define BUFFER_SIZE 8 * 1024 * 1024

#define MTU_SIZE 1500

TCB::TCB() {
    sendBuf = new char[BUFFER_SIZE];
    recvBuf = new char[BUFFER_SIZE];

    currentSegment = new char[MTU_SIZE];
}

TCB::~TCB() {
    delete[] sendBuf;
    delete[] recvBuf;
    delete[] currentSegment;
}

void TCB::updateState(ConnectionState state) { myState.updateState(state); }

ACTION TCB::updateState(char* buf, int size) { return myState.updateState(buf, size); }

ConnectionState TCB::getState() { return myState.getState(); }
