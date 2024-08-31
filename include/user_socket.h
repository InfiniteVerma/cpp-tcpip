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

    static int bindSocket(); 
private:
    UserSocket();
    ~UserSocket();
};

#endif
