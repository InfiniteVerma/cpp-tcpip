#ifndef __RFC793_STATEMACHINE__
#define __RFC793_STATEMACHINE__

#include <ostream>
#include <vector>

#include "packet.h"
enum ConnectionState {
    LISTEN = 0,
    OPEN,
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

/*
 * If checker is NULL, it's always true (default case when no checks are needed)
 * If action is NULL, that means no action == discard that packet
 */

// clang-format off
const vector<StateMData> FSM = {
    StateMData(CLOSED, Packet::isRstPacket, NULL, CLOSED),
    StateMData(CLOSED, Packet::isNotRstPacket, Packet::getRSTPacket, CLOSED),

    StateMData(OPEN, NULL, Packet::getSYNPacket, SYN_SENT),

    StateMData(LISTEN, Packet::isRstPacket, NULL, LISTEN),
    StateMData(LISTEN, Packet::isAckPacket, Packet::getRSTPacket, LISTEN),
    StateMData(LISTEN, Packet::isSynPacket, Packet::getSynAckPacket, SYN_RECEIVED),
    StateMData(LISTEN, NULL, NULL, LISTEN),

    StateMData(SYN_SENT, Packet::isSynAckPacket, Packet::getAckPacket, ESTABLISHED),
    StateMData(SYN_SENT, Packet::isRstPacket, NULL, CLOSED), // TODO how to release resources??

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
