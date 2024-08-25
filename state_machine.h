#ifndef __RFC793_STATEMACHINE__
#define __RFC793_STATEMACHINE__

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

    int getState();
    void updateState();
    void updateState(ConnectionState);

private:
    ConnectionState state;
};

#endif
