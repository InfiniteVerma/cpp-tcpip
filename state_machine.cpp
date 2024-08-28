#include "state_machine.h"

#include <cassert>
#include <iostream>

#include "packet.h"

TCBStateM::TCBStateM() { state = CLOSED; }

ConnectionState TCBStateM::getState() { return state; }

ACTION TCBStateM::updateState(char *pkt, int size) {
    cout << __FUNCTION__ << " BEGIN current State: " << state << "\n";

    if (!pkt && state == CLOSED)  // TODO better way?
    {
        state = SYN_SENT;
        return FSM[0].action;
    }
    // Parse packet
    Packet parsedPacket = Packet(pkt, size);

    // iterate through FSM, find matching current state. If not match?
    for (auto itr : FSM) {
        if (itr.currentState == state) {
            cout << "Found matching state: " << state << "\n";
            if (itr.checker(parsedPacket)) {
                cout << "Checker passed! Updating state\n";
                state = itr.nextState;
                return itr.action;
            }
        }
    }
    cout << "Could not find a valid ACTION, returning NULL\n";
    assert(0);
    return NULL;
}

void TCBStateM::updateState(ConnectionState state) { this->state = state; }

std::ostream &operator<<(std::ostream &os, const ConnectionState &state) {
    switch (state) {
        case LISTEN:
            os << "LISTEN";
            break;
        case SYN_SENT:
            os << "SYN_SENT";
            break;
        case SYN_RECEIVED:
            os << "SYN_RECEIVED";
            break;
        case ESTABLISHED:
            os << "ESTABLISHED";
            break;
        case FIN_WAIT_1:
            os << "FIN_WAIT_1";
            break;
        case FIN_WAIT_2:
            os << "FIN_WAIT_2";
            break;
        case CLOSE_WAIT:
            os << "CLOSE_WAIT";
            break;
        case CLOSING:
            os << "CLOSING";
            break;
        case LAST_ACK:
            os << "LAST_ACK";
            break;
        case TIME_WAIT:
            os << "TIME_WAIT";
            break;
        case CLOSED:
            os << "CLOSED";
            break;
        default:
            os << "ERROR GARBAGE VALUE!";
            break;
    }
    return os;
}
