#ifndef __RFC793_TCP__
#define __RFC793_TCP__

#include "common.h"

/*
 * TCP Header format
 * -----------------
 *
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |          Source Port          |       Destination Port        |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                        Sequence Number                        |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                    Acknowledgment Number                      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |  Data |           |U|A|P|R|S|F|                               |
 *  | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
 *  |       |           |G|K|H|T|N|N|                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |           Checksum            |         Urgent Pointer        |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                    Options                    |    Padding    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                             data                              |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

struct TCPHeader {
    UINT16 source_port;
    UINT16 dest_port;
    UINT32 seq_number;
    UINT32 ack_number;

    /*
     * Control Bits:  6 lower bits (from left to right):
     *
     *  - URG:  Urgent Pointer field significant
     *  - ACK:  Acknowledgment field significant
     *  - PSH:  Push Function
     *  - RST:  Reset the connection
     *  - SYN:  Synchronize sequence numbers
     *  - FIN:  No more data from sender
     */
    UINT16 data_offset_and_flags;  // MSB 4 bits + 6 bits 0 + 6 bits of flag

    UINT16 window;
    UINT16 checksum;
    UINT16 urgent_pointer;
    // UINT8 options; // TODO. should be multiples of 8 ideally?

    TCPHeader(int source_p, int dest_p) {
        source_port = source_p;
        dest_port = dest_p;
        seq_number = 0;
        ack_number = 0;
        data_offset_and_flags = 0;
        window = 0;
        checksum = 0;
        urgent_pointer = 0;
        // options = 0;
    }
};

#endif
