#ifndef __RFC793_USER_SOCKET__
#define __RFC793_USER_SOCKET__

#include <sys/types.h>

#include <set>
#include <string>

#include "common.h"

/*
 * This exposed to the user.
 */
class UserSocket {
   public:
    static UINT8 create(std::string s, std::string srcIp, std::string destIp, int port);
    static int bind(UINT8 fd);
    static int listen(UINT8 fd, int backlog);
    static int connect(UINT8 fd);
    static int accept(UINT8 fd);
    static int close(UINT8 fd);

    static int send(UINT8 fd, const void* buffer, size_t length, int flags);
    static int receive(UINT8 fd, char* buffer, ssize_t length, int flags);

    static int stopTCP();

   private:
    UserSocket();
    ~UserSocket();
};

#endif
