#include "user_socket.h"

#include <stdlib.h>
#include <sys/msg.h>

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "messages.h"
#include "mytcp.h"

/*
 * This call sends a request to the mytcp thread who will in time respond back with a fd
 * Further stuff will be called with key as the fd (similar to how tcp/ip works in the linux kernel)
 *
 */
UINT8 UserSocket::create(std::string s, std::string srcIp, std::string destIp, int port) {
    char *logTypeEnvVal = getenv("LOGTYPE");
    Utils::writeToLogFile = true;
    Utils::logFileName = s + ".log";

    if (!logTypeEnvVal || strcmp(logTypeEnvVal, "STDOUT") == 0) Utils::writeToLogFile = false;

    int msgQueueID = MyTcp::getMsgQueueID();

    LOG("MsgQueueID: ", msgQueueID);

    MyMsg msg(CREATE_SOCKET, s.c_str(), s.length(), srcIp.c_str(), srcIp.length(), destIp.c_str(), destIp.length(),
              port);

    // First send the request on the message queue
    int ret = msgsnd(msgQueueID, &msg, sizeof(MyMsg), 0);

    if (ret == -1) {
        printf("Oh dear, something went wrong with msgsnd()! %s\n", strerror(errno));
        assert(0);
    }

    UINT8 fd = MyTcp::getFD();

    // Then wait for the DS to have an FD (key: PORT?)
    return fd;  // TODO
}

int UserSocket::bind(UINT8 fd) {
    LOG(__FUNCTION__, " BEGIN: <", static_cast<int>(fd), ">\n");

    MyMsg msg(BIND_SOCKET, fd);
    msg.fd = static_cast<UINT8>(fd);

    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);

    if (ret != -1) {
        ret = MyTcp::getRetval();
    }
    return ret;
}

/*
 * Blocks until a client completes 3-way handshake.
 *
 * TODO return it's fd. Currently dummy since we have one socket on each side.
 */
int UserSocket::accept(UINT8 fd) {
    LOG(__FUNCTION__, " BEGIN fd: ", fd);
    MyMsg msg(ACCEPT_SOCKET, fd);

    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);
    if (ret == -1) {
        LOG(__FUNCTION__, "ERROR ret == -1, error code", strerror(ret));
    }

    if (ret != -1) {
        ret = MyTcp::getRetval();
    }

    LOG(__FUNCTION__, " getRetval returned ret: ", ret);
    return ret;
}

/*
 * Dummy rn. Just sends the msg that's received by the tcp thread.
 *
 * TODO use backlog
 */
int UserSocket::listen(UINT8 fd, int backlog) {
    LOG(__FUNCTION__, " BEGIN backlog not being used TODO: ", backlog);

    MyMsg msg(LISTEN_SOCKET, fd);

    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);
    if (ret == -1) {
        LOG(__FUNCTION__, "ERROR ret == -1, error code", strerror(ret));
    }

    LOG(__FUNCTION__, " TODO send the socket fd back. Not doing this rn!!");

    return ret;
}

/*
 * Starts the 3-Way handshake by sending a SYN packet and starts a timer of 5 seconds.
 * If handshake doesn't complete by then, return an error.
 */
int UserSocket::connect(UINT8 fd) {
    LOG(__FUNCTION__, " BEGIN fd: ", fd);
    MyMsg msg(CONNECT_SOCKET, fd);
    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);

    if (ret == -1) {
        LOG(__FUNCTION__, "ERROR ret == -1, error code", strerror(ret));
    } else {
        LOG(__FUNCTION__, " waiting for retval from TCP thread");
        ret = MyTcp::getRetval();
    }

    LOG(__FUNCTION__, " returning ret: ", ret);
    return ret;
}

int UserSocket::close(UINT8 fd) {
    MyMsg msg(CLOSE_SOCKET, fd);
    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);

    if (ret == -1) {
        LOG(__FUNCTION__, "ERROR ret == -1, error code", strerror(ret));
    } else {
        ret = MyTcp::getRetval();
    }

    return ret;
}

int UserSocket::send(UINT8 fd, const void *buffer, size_t size, int flags) {
    LOG(__FUNCTION__, " sending data to socket. TODO flags are ignored");
    int ret = MyTcp::insertPacketInSendBuffer(buffer, size);

    if (ret == -1) {
        LOG(__FUNCTION__, " ERROR no space available!");
        assert(0);
    }

    MyMsg msg(SEND_PACKET, fd);
    msg.slotIndex = ret;
    msg.packetSize = size;

    ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);

    if (ret == -1) {
        LOG(__FUNCTION__, "ERROR ret == -1, error code", strerror(ret));
    } else {
        // ret = MyTcp::getRetval();
    }

    LOG(__FUNCTION__, " packet inserted in buffer and message sent!");
    return ret;
}

int UserSocket::stopTCP() {
    LOG(__FUNCTION__, "sending a message to tcp thread to stop it");
    MyMsg msg(STOP_TCP_THREAD);
    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);

    if (ret == -1) {
        LOG(__FUNCTION__, "ERROR ret == -1, error code", strerror(ret));
    }

    MyTcp::waitForThreadToDie();

    return ret;
}

// TODO use fd
int UserSocket::receive(UINT8 fd, char *buffer, ssize_t length, int flags) {
    LOG(__FUNCTION__, " BEGIN, blocking to wait for a packet in buffer");
    pair<int, int> slotAndSize = MyTcp::waitForMessageInBuffer();
    LOG(__FUNCTION__, " notified that there's a packet in buffer, slot: ", slotAndSize.first,
        " of size: ", slotAndSize.second);
    return MyTcp::getPacketFromBuffer(buffer, slotAndSize.first, slotAndSize.second);
}
