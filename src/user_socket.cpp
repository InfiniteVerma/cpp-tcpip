#include "user_socket.h"
#include <sys/msg.h>

/*
 * This call sends a request to the mytcp thread who will in time respond back with a fd
 * Further stuff will be called with key as the fd (similar to how tcp/ip works in the linux kernel)
 *
 */
int UserSocket::createSocket(std::string s, const char *ip, int port)
{
    // First send the request on the message queue
    int ret = msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)

    // Then wait for the DS to have an FD (key: PORT?)
}
