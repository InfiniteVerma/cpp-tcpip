#include <iostream>
#include "socket.h"

int main()
{
    std::cout << "Hello World\n";
    Socket socket(8000);
    socket.debugPrint();

    socket.bind();
    socket.listen();
    return 0;
}
