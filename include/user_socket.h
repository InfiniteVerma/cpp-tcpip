#ifndef __RFC793_USER_SOCKET__
#define __RFC793_USER_SOCKET__

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
    static int listen(UINT8 fd);
    static int connect(UINT8 fd);
    static int close(UINT8 fd);
    static int send(UINT8 fd, const void* buffer, size_t length, int flags);

    static int stopTCP();

   private:
    UserSocket();
    ~UserSocket();
};

#endif
