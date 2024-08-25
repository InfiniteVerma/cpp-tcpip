#ifndef __RFC793_PACKET__
#define __RFC793_PACKET__

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
    UINT8 data_offset; // 4 bits (lower 4) only
    UINT8 reserved; // 6 lower bits. Must be 0

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
    UINT8 control_bits;
    UINT16 window;
    UINT16 checksum;
    UINT16 urgent_pointer;
    UINT8 options; // TODO. should be multiples of 8 ideally?
};

/*
 * Constructs a packet (payload + header)
 */
class Packet {
};

#endif
