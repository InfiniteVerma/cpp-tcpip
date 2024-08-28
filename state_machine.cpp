#include "state_machine.h"
#include <cassert>
#include <iostream>

TCBStateM::TCBStateM() { state = CLOSED; }

ConnectionState TCBStateM::getState() { return state; }

void TCBStateM::updateState(char *pkt) {
  // TODO
}

void TCBStateM::updateState(ConnectionState state) {
  if (state != LISTEN) {
    assert(0);
  }

  this->state = state;
}

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
