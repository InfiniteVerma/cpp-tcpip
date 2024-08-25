#### cpp-tcpip

A tcp/ip stack in cpp, implementing RFC793.

#### Plan

- [x] Read RFC793 few times
- [ ] Boilerplate code
      - Make your own socket API calls
      - TCB code
- [ ] State machine
- [ ] Do 3 way handshake and maintain the state (of FSM) properly
- [ ] Try sending a packet after connection is established
- [ ] Timer logic 
- [ ] Sequence window logic
- [ ] Retransmission logic
- [ ] First test

End goal: Sending a file as a byte stream over this tcp/ip implementation

#### References
 - https://datatracker.ietf.org/doc/html/rfc793
