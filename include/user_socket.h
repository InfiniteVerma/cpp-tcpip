#ifndef __RFC793_USER_SOCKET__
#define __RFC793_USER_SOCKET__

#include <set>
#include <string>

/*
 * This exposed to the user.
 */
class UserSocket {
   public:
    static int createSocket(std::string s, std::string srcIp, std::string destIp, int port);

    static int bindSocket(int fd);
    static int listenSocket(int fd);

   private:
    UserSocket();
    ~UserSocket();
};

#endif
