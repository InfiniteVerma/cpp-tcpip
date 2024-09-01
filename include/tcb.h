#ifndef __RFC793_TCB__
#define __RFC793_TCB__

#include <queue>

#include "common.h"
#include "state_machine.h"

class TCB {
   public:
    UINT16 localPortNum;                     /* local port num */
    UINT16 remotePortNum;                    /* remote port num */
    char *sendBuf;                           /* send buffer */
    char *recvBuf;                           /* receive buffer */
    std::queue<char *> retransmissionQueue;  // TODO MAX SIZE?
    char *currentSegment;                    // TODO what's this?

    // Page 20
    /* send sequence variables */
    UINT32 snd_una;
    UINT32 snd_nxt;
    UINT32 snd_wnd;
    UINT32 snd_up;
    UINT32 snd_wl1;
    UINT32 snd_wl2;
    UINT32 iss;  // initial send sequence number

    /* receive sequence variables */
    UINT32 rcv_nxt;
    UINT32 rcv_wnd;
    UINT32 rcv_up;
    UINT32 irs;

    /* current segment variables */
    UINT32 seg_seq;
    UINT32 seq_ack;
    UINT32 seg_len;
    UINT32 seg_wnd;
    UINT32 seg_up;
    UINT32 seg_prc;

    ConnectionState getState();
    void updateState(ConnectionState state);
    ACTION updateState(const char *buf, int size);
    void generateISN();
    void debugPrint();

    /* my connection state */

    TCB();
    ~TCB();

   private:
    TCBStateM myState;
};

#endif
