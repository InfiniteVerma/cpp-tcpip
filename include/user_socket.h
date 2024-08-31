#ifndef __RFC793_USER_SOCKET__
#define __RFC793_USER_SOCKET__

#include <set>
#include <string>

/*
 * This exposed to the user.
 */
class UserSocket {
   public:
    static int create(std::string s, std::string srcIp, std::string destIp, int port);

    static int bind(int fd);
    static int listen(int fd);
    static int connect(int fd);

   private:
    UserSocket();
    ~UserSocket();
};

#endif
