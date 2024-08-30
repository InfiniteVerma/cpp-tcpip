#ifndef __RFC793_MESSAGE__
#define __RFC793_MESSAGE__

enum MessageTypes {
    CREATE_SOCKET = 0,
    CLOSE_SOCKET,
};

struct mymsg {
    MessageTypes mtype;
    char   mtext[1];
};

#endif
