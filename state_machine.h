#ifndef __RFC793_STATEMACHINE__
#define __RFC793_STATEMACHINE__

#include <ostream>
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

class TCBStateM {
  public:
    TCBStateM();

    ConnectionState getState();
    void updateState(char *pkt, int size);
    void updateState(ConnectionState);

  private:
    ConnectionState state;
};

std::ostream &operator<<(std::ostream &os, const ConnectionState &state);

#endif
