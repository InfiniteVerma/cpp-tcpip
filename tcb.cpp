#include "tcb.h"

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
