#ifndef __RFC792_IP__
#define __RFC793_IP__

#include "common.h"
#include <arpa/inet.h>
#include <netinet/in.h>

/*
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |Version|  IHL  |Type of Service|          Total Length         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |         Identification        |Flags|      Fragment Offset    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |  Time to Live |    Protocol   |         Header Checksum       |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                       Source Address                          |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                    Destination Address                        |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                    Options                    |    Padding    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

struct IPHeader {
    UINT8 idl : 4;
    UINT8 version : 4;
    UINT8 type_of_service;
    UINT16 total_length;
    UINT16 identification;
    UINT16 fragment_offset; // includes 3 bits of flags
    UINT8 ttl;
    UINT8 protocol;
    UINT16 checksum;
    UINT32 source_addr;
    UINT32 dest_addr;
    // TODO options?

    IPHeader() {
        idl = 5; // TODO calculate this
        version = 4;
        type_of_service = 0;
        total_length = 0;              // Will be updated after payload is added
        identification = htons(54321); // what is this?
        fragment_offset = 0;
        ttl = 64;
        protocol = IPPROTO_RAW;
        checksum = 0; // Will be updated later
        source_addr = inet_addr("127.0.0.1");
        dest_addr = inet_addr("127.0.0.1"); // TODO ?
    }
};

/*
 * The checksum algorithm is:
 *
 *  The checksum field is the 16 bit one's complement of the one's
 *  complement sum of all 16 bit words in the header.  For purposes of
 *  computing the checksum, the value of the checksum field is zero.
 */
unsigned int getChecksumVal(void *ipHeader);

#endif
