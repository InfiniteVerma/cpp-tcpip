#include <iostream>
#include "tcp.h"

int main()
{
    std::cout << "Hello World\n";
    Socket socket(8000);
    socket.debugPrint();
    return 0;
}
