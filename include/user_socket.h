#ifndef __RFC793_USER_SOCKET__
#define __RFC793_USER_SOCKET__

#include <set>
#include <string>

/*
 * This exposed to the user. 
 */
class UserSocket {
    int createSocket(std::string s, const char *ip, int port);
private:
    UserSocket();
    ~UserSocket();
};

#endif
