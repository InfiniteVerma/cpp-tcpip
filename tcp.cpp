#include "tcp.h"
#include "iostream"
using namespace std;

Socket::Socket(int p) : port(p) {}

void
Socket::debugPrint()
{
    cout << "Socket debugPrint\n";
    cout << "Port: " << port << "\n";
}
