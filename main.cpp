#include <iostream>
#include "socket.h"

int main()
{
    std::cout << "Hello World\n";
    Socket socket(8000);

    socket.bind();
    socket.listen();
    socket.debugPrint();
    return 0;
}
