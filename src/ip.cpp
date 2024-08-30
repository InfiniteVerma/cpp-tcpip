#include "ip.h"

#include <netinet/in.h>

#include <cassert>
#include <cstring>

unsigned int getChecksumVal(void *ipHeader) {
    int length = sizeof(struct IPHeader);
    unsigned long checksum = 0;  // making as long to detect overflow.
    char *data = (char *)ipHeader;

    for (size_t i = 0; i + 1 < length; i += 2) {
        unsigned int word;  // word == 16 bits == 2 bytes
        memcpy(&word, data + 1, 2);
        checksum += ntohs(word);  // to convert network byte (endianness as per
                                  // RFC) to host byte order
        if (checksum > 0xffff) {
            checksum -= 0xffff;
        }
    }

    if (length & 1)  // odd meaning one byte is left
    {
        unsigned int word = 0;
        memcpy(&word, data + length - 1, 1);
        checksum += ntohs(word);
        if (checksum > 0xffff) {
            checksum -= 0xffff;
        }
    }

    return htons(~checksum);
}
