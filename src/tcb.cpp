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
    //    delete[] sendBuf;
    //    delete[] recvBuf;
    //    delete[] currentSegment;
}

void TCB::updateState(ConnectionState state) { myState.updateState(state); }

ACTION TCB::updateState(char* buf, int size) { return myState.updateState(buf, size); }

void TCB::generateISN() {
    const auto now = chrono::system_clock::now();
    const auto epoch = now.time_since_epoch();
    const auto millis = chrono::duration_cast<chrono::milliseconds>(epoch).count();

    snd_nxt = millis % (1 << 31);

    cout << "Generated ISN: " << snd_nxt << "\n";
}

ConnectionState TCB::getState() { return myState.getState(); }

void TCB::debugPrint() {
    cout << "=========\nTCB Data\n";
    cout << "snd_una: " << snd_una << "\n";
    cout << "snd_nxt: " << snd_nxt << "\n";
    cout << "snd_wnd: " << snd_wnd << "\n";
    cout << "snd_up: " << snd_up << "\n";
    cout << "snd_wl1: " << snd_wl1 << "\n";
    cout << "snd_wl2: " << snd_wl2 << "\n";
    cout << "iss: " << iss << "\n";
    cout << "rcv_nxt: " << rcv_nxt << "\n";
    cout << "rcv_wnd: " << rcv_wnd << "\n";
    cout << "rcv_up: " << rcv_up << "\n";
    cout << "irs: " << irs << "\n";
    cout << "seg_seq: " << seg_seq << "\n";
    cout << "seq_ack: " << seq_ack << "\n";
    cout << "seg_len: " << seg_len << "\n";
    cout << "seg_wnd: " << seg_wnd << "\n";
    cout << "seg_up: " << seg_up << "\n";
    cout << "seg_prc: " << seg_prc << "\n";
    cout << "My State: " << myState.getState() << "\n";
}
