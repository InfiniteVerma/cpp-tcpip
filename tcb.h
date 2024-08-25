#ifndef __RFC793_TCP__
#define __RFC793_TCP__

#include <cstdint>
#include <queue>
#include "state_machine.h"

typedef uint16_t UINT16;
typedef uint32_t UINT32;

/*
 * TCP Header format
 *
 *     0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          Source Port          |       Destination Port        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Sequence Number                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Acknowledgment Number                      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Data |           |U|A|P|R|S|F|                               |
   | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
   |       |           |G|K|H|T|N|N|                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Checksum            |         Urgent Pointer        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             data                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

class TCB {
public:
    UINT16 localPortNum; /* local port num */
    UINT16 remotePortNum; /* remote port num */
    char* sendBuf; /* send buffer */
    char* recvBuf; /* receive buffer */
    std::queue<char*> retransmissionQueue; // TODO MAX SIZE?
    char* currentSegment; // TODO what's this?

    // Page 20
    /* send sequence variables */
    UINT32 snd_una;
    UINT32 snd_nxt;
    UINT32 snd_wnd;
    UINT32 snd_up;
    UINT32 snd_wl1;
    UINT32 snd_wl2;
    UINT32 iss;

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

    /* my connection state */
    TCBStateM myState;

    TCB();
    ~TCB();
};

#endif
