#ifndef __RFC793_STATEMACHINE__
#define __RFC793_STATEMACHINE__

#include "packet.h"
#include <ostream>
#include <vector>
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
typedef Packet (*ACTION)(PktData); // TODO?

struct StateMData {
    ConnectionState currentState;
    CHECKER checker;
    ACTION action;
    ConnectionState nextState;

    StateMData(ConnectionState currS, CHECKER checker, ACTION action,
               ConnectionState nextS) {
        currentState = currS;
        this->checker = checker;
        nextState = nextS;
        this->action = action;
    }
};

const vector<StateMData> FSM = {
    StateMData(CLOSED, NULL, Packet::getSYNPacket, SYN_SENT),
    StateMData(LISTEN, Packet::isSYNPacket, Packet::getSynAckPacket,
               SYN_RECEIVED)};

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
