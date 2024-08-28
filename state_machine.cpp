#include "state_machine.h"
#include "packet.h"
#include <cassert>
#include <iostream>

TCBStateM::TCBStateM() { state = CLOSED; }

ConnectionState TCBStateM::getState() { return state; }

ACTION TCBStateM::updateState(char *pkt, int size) {
    cout << __FUNCTION__ << " BEGIN\n";
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
    return NULL;

    ///*
    // * If receive SYN, send some
    // */
    // if (Packet::isSYNPacket(parsedPacket) && getState() == LISTEN) {
    //    cout << "Socket is in LISTEN state and got SYN type packet!\n";
    //}
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
