#ifndef __RFC793_MESSAGE__
#define __RFC793_MESSAGE__

#include <cassert>
#include <cstring>

#include "common.h"
enum MessageTypes {
    CREATE_SOCKET = 0,
    CLOSE_SOCKET,
    BIND_SOCKET,
    LISTEN_SOCKET,
    CONNECT_SOCKET,
    STOP_TCP_THREAD,
    SEND_PACKET,
    ERROR,
};

struct MyMsg {
    MessageTypes mtype;
    UINT8 fd;
    char socketName[11];
    char sourceIpAddr[16];
    char destIpAddr[16];
    UINT16 port;
    UINT8 slotIndex;
    UINT16 packetSize;

    MyMsg(MessageTypes mtype, int fd) {
        this->mtype = mtype;
        this->fd = static_cast<UINT8>(fd);
        // this->socketName[0] = '\0';
        // this->sourceIpAddr[0] = '\0';
        // this->destIpAddr[0] = '\0';
        // this->port = 0;
    }

    MyMsg(MessageTypes mtype, const char* socketName, int socketNameLen, const char* sourceIpAddr, int sourceIpAddrLen,
          const char* destIpAddr, int destIpAddrLen, UINT16 port) {
        assert(("Invalid length" && (socketNameLen < 11 || sourceIpAddrLen < 16)));

        this->mtype = mtype;

        this->fd = 0;
        strncpy(this->socketName, socketName, socketNameLen);
        this->socketName[socketNameLen] = '\0';

        strncpy(this->sourceIpAddr, sourceIpAddr, sourceIpAddrLen);
        this->sourceIpAddr[sourceIpAddrLen] = '\0';

        strncpy(this->destIpAddr, destIpAddr, destIpAddrLen);
        this->destIpAddr[destIpAddrLen] = '\0';

        this->port = port;
    }

    MyMsg() {}

    MyMsg(MessageTypes mtype) { this->mtype = mtype; }

    void print() {
        LOG("======MyMsg========");
        LOG("mtype: ", (MessageTypes)mtype);

        switch (mtype) {
            case SEND_PACKET: {
                LOG("fd: ", static_cast<int>(fd));
                LOG("slotindex: ", slotIndex);
                LOG("packet size: ", packetSize);
                break;
            }
            default: {
                LOG("socketName: ", socketName);
                LOG("sourceIpAddr: ", sourceIpAddr);
                LOG("destIpAddr: ", destIpAddr);
                LOG("port: ", port);
                LOG("fd: ", static_cast<int>(fd));
                break;
            }
        }
    }
};
#endif
