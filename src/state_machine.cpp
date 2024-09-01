#include "state_machine.h"

#include <cassert>
#include <iostream>

#include "packet.h"

TCBStateM::TCBStateM() { state = CLOSED; }

ConnectionState TCBStateM::getState() { return state; }

ACTION TCBStateM::updateState(const char *pkt, int size) {
    LOG(__FUNCTION__, " BEGIN current State: ", state);

    if (!pkt && state == CLOSED)  // TODO better way?
    {
        assert(0);
        // TODO remove
        state = SYN_SENT;
        return FSM[0].action;
    }

    if (pkt) {
        LOG(__FUNCTION__, " Packet pushing to FSM:");
        Packet p = Packet(pkt, size);
        p.printPacket();
    }

    // iterate through FSM, find matching current state.
    for (auto itr : FSM) {
        if (itr.currentState == state) {
            LOG("Found matching state: ", state);
            if (!itr.checker) {
                LOG("Checker is NULL! Updating state");
                state = itr.nextState;
                return itr.action;
            }

            assert(pkt);
            Packet parsedPacket = Packet(pkt, size);

            if (itr.checker(parsedPacket)) {
                LOG("Checker passed! Updating state");
                state = itr.nextState;
                return itr.action;
            } else {
                continue;
            }
        }
    }

    if (getState() != ESTABLISHED) {
        LOG("Could not find a valid ACTION and not in estalished state. Not handling this case rn");  // TODO
        assert(0);
    }
    LOG("Could not find a valid ACTION, returning NULL, assuming it's a data pkt");
    return NULL;
}

void TCBStateM::updateState(ConnectionState state) { this->state = state; }

std::ostream &operator<<(std::ostream &os, const ConnectionState &state) {
    switch (state) {
        case LISTEN:
            os << "LISTEN";
            break;
        case OPEN:
            os << "OPEN";
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
