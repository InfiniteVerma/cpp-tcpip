#include "tcb.h"

#include <chrono>

#include "state_machine.h"

#define BUFFER_SIZE 8 * 1024 * 1024

#define MTU_SIZE 1500

TCB::TCB() {
    sendBuf = new char[BUFFER_SIZE];
    recvBuf = new char[BUFFER_SIZE];
    currentSegment = new char[MTU_SIZE];
    snd_una = 0;
    snd_nxt = 0;
    snd_wnd = 0;
    snd_up = 0;
    snd_wl1 = 0;
    snd_wl2 = 0;
    iss = 0;
    rcv_nxt = 0;
    rcv_wnd = 0;
    rcv_up = 0;
    irs = 0;
    seg_seq = 0;
    seq_ack = 0;
    seg_len = 0;
    seg_wnd = 0;
    seg_up = 0;
    seg_prc = 0;

    generateISN();
}

TCB::~TCB() {
    // TODO fix below
    delete[] sendBuf;
    delete[] recvBuf;
    delete[] currentSegment;
}

void TCB::updateState(ConnectionState state) { myState.updateState(state); }

ACTION TCB::updateState(char* buf, int size) { return myState.updateState(buf, size); }

void TCB::generateISN() {
    // const auto now = chrono::system_clock::now();
    // const auto epoch = now.time_since_epoch();
    // const auto millis = chrono::duration_cast<chrono::milliseconds>(epoch).count();

    // snd_nxt = millis % (1 << 31);
    iss = 1;

    LOG("Generated ISN: ", iss);
}

ConnectionState TCB::getState() { return myState.getState(); }

void TCB::debugPrint() {
    LOG("=========\nTCB Data");
    // LOG("snd_una: ", snd_una);
    // LOG("snd_nxt: ", snd_nxt);
    // LOG("snd_wnd: ", snd_wnd);
    // LOG("snd_up: ", snd_up);
    // LOG("snd_wl1: ", snd_wl1);
    // LOG("snd_wl2: ", snd_wl2);
    // LOG("iss: ", iss);
    // LOG("rcv_nxt: ", rcv_nxt);
    // LOG("rcv_wnd: ", rcv_wnd);
    // LOG("rcv_up: ", rcv_up);
    // LOG("irs: ", irs);
    // LOG("seg_seq: ", seg_seq);
    // LOG("seq_ack: ", seq_ack);
    // LOG("seg_len: ", seg_len);
    // LOG("seg_wnd: ", seg_wnd);
    // LOG("seg_up: ", seg_up);
    // LOG("seg_prc: ", seg_prc);
    LOG("My State: ", myState.getState());
}
