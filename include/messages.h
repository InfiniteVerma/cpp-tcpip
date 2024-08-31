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
    ERROR,
};

struct MyMsg {
    MessageTypes mtype;
    UINT8 fd;
    char socketName[11];
    char sourceIpAddr[16];
    char destIpAddr[16];
    UINT16 port;

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

    MyMsg(MessageTypes mtype, int fd) { this->mtype = mtype; }

    void print() {
        cout << "======MyMsg========\n";
        cout << "mtype: " << (MessageTypes)mtype << "\n"
             << "socketName: " << socketName << "\n"
             << "sourceIpAddr: " << sourceIpAddr << "\n"
             << "destIpAddr: " << destIpAddr << "\n"
             << "port: " << port << "\n"
             << "fd: " << fd << "\n";
    }
};

#endif