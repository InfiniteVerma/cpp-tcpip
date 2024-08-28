#include "state_machine.h"
#include "packet.h"
#include <cassert>
#include <iostream>

TCBStateM::TCBStateM() { state = CLOSED; }

ConnectionState TCBStateM::getState() { return state; }

void TCBStateM::updateState(char *pkt, int size) {
    // Parse packet
    Packet parsedPacket = Packet(pkt, size);

    cout << "isSYN: " << parsedPacket.isSYN() << "\n";
    cout << "getState: " << getState() << "\n";

    /*
     * If receive SYN, send some
     */
    if (parsedPacket.isSYN() && getState() == LISTEN) {
        cout << "Socket is in LISTEN state and got SYN type packet!\n";
    }
}

void TCBStateM::updateState(ConnectionState state) { this->state = state; }

std::ostream &operator<<(std::ostream &os, const ConnectionState &state) {
    switch (state) {
    case LISTEN:
        os << "LISTEN";
        break;
    default:
        os << "UNDEFINED!";
        break;
    }
    return os;
}
