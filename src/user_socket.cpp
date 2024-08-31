#include "user_socket.h"
#include <sys/msg.h>
#include "messages.h"
#include "mytcp.h"
#include <cassert>
#include <cstring>

/*
 * This call sends a request to the mytcp thread who will in time respond back with a fd
 * Further stuff will be called with key as the fd (similar to how tcp/ip works in the linux kernel)
 *
 */
int UserSocket::createSocket(std::string s, std::string srcIp, std::string destIp, int port)
{
    int msgQueueID = MyTcp::getMsgQueueID();

    MyMsg msg(CREATE_SOCKET, s.c_str(), s.length(), srcIp.c_str(), srcIp.length(), destIp.c_str(), destIp.length(), port);

    // First send the request on the message queue
    int ret = msgsnd(msgQueueID, &msg, sizeof(msg), 0);
    assert("msgsnd failed" && ret != -1);

    // Then wait for the DS to have an FD (key: PORT?)
    return 1; // TODO
}
