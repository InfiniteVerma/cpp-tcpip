#ifndef __RFC793_STATEMACHINE__
#define __RFC793_STATEMACHINE__

#include <ostream>
#include <vector>

#include "packet.h"
enum ConnectionState {
    LISTEN = 0,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT,
    CLOSED
};

typedef bool (*CHECKER)(Packet);
typedef Packet (*ACTION)(PktData);  // TODO?

struct StateMData {
    ConnectionState currentState;
    CHECKER checker;
    ACTION action;
    ConnectionState nextState;

    StateMData(ConnectionState currentState, CHECKER checker, ACTION action, ConnectionState nextState) {
        this->currentState = currentState;
        this->checker = checker;
        this->nextState = nextState;
        this->action = action;
    }
};

// A connection progresses through a series of states during its
//  lifetime.  The states are:  LISTEN, SYN-SENT, SYN-RECEIVED,
//  ESTABLISHED, FIN-WAIT-1, FIN-WAIT-2, CLOSE-WAIT, CLOSING, LAST-ACK,
//  TIME-WAIT, and the fictional state CLOSED.  CLOSED is fictional
//  because it represents the state when there is no TCB, and therefore,
//  no connection.  Briefly the meanings of the states are:

// clang-format off
const vector<StateMData> FSM = {
    StateMData(CLOSED, NULL, Packet::getSYNPacket, SYN_SENT),
    StateMData(LISTEN, Packet::isSynPacket, Packet::getSynAckPacket, SYN_RECEIVED),
    StateMData(LISTEN, Packet::isAckPacket, Packet::getRSTPacket, LISTEN),
    StateMData(SYN_SENT, Packet::isSynAckPacket, Packet::getAckPacket, ESTABLISHED),
    StateMData(SYN_RECEIVED, Packet::isAckPacket, NULL, ESTABLISHED)
};
// clang-format on

class TCBStateM {
   public:
    TCBStateM();

    ConnectionState getState();
    ACTION updateState(char *pkt, int size);
    void updateState(ConnectionState);

   private:
    ConnectionState state;
};

std::ostream &operator<<(std::ostream &os, const ConnectionState &state);

#endif
