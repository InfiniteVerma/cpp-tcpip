#include "user_socket.h"

#include <sys/msg.h>

#include <cassert>
#include <cstring>

#include "messages.h"
#include "mytcp.h"

/*
 * This call sends a request to the mytcp thread who will in time respond back with a fd
 * Further stuff will be called with key as the fd (similar to how tcp/ip works in the linux kernel)
 *
 */
UINT8 UserSocket::create(std::string s, std::string srcIp, std::string destIp, int port) {
    int msgQueueID = MyTcp::getMsgQueueID();

    cout << "MsgQueueID: " << msgQueueID << "\n";

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
    cout << __FUNCTION__ << " BEGIN: <" << static_cast<int>(fd) << ">\n";

    MyMsg msg(BIND_SOCKET, fd);
    msg.fd = static_cast<UINT8>(fd);

    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);

    if (ret != -1) {
        ret = MyTcp::getRetval();
    }
    return ret;
}

int UserSocket::listen(UINT8 fd) {
    MyMsg msg(LISTEN_SOCKET, fd);

    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);
    cout << ret << "\n";

    if (ret != -1) {
        ret = MyTcp::getRetval();
    }
    return ret;
}

/*
 * Starts the 3-Way handshake by sending a SYN packet and starts a timer of 5 seconds. 
 * If handshake doesn't complete by then, return an error.
 */ 
int UserSocket::connect(UINT8 fd)
{
    MyMsg msg(CONNECT_SOCKET, fd);
    int ret = msgsnd(MyTcp::getMsgQueueID(), &msg, sizeof(MyMsg), 0);
    
    cout << ret << "\n";

    if (ret != -1) {
        ret = MyTcp::getRetval();
    }
    cout << "======\nANANT Get ret value from getRetval: " << ret << "\n";
    return ret;
}
