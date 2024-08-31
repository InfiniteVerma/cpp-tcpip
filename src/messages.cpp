#include "messages.h"

std::ostream &operator<<(std::ostream &os, const MessageTypes &msgType) {
    switch (msgType) {
        case CREATE_SOCKET:
            os << "CREATE_SOCKET";
            break;
        case CLOSE_SOCKET:
            os << "CLOSE_SOCKET";
            break;
        case ERROR:
            os << "ERROR";
            break;
        default:
            os << "ERROR GARBAGE VALUE of msgtype!";
            break;
    }
    return os;
}
