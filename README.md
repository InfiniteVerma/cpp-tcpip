#### cpp-tcpip

A tcp/ip stack in cpp, implementing RFC793.

#### Steps

Add the two ips in lo loopback interface

```
sudo ip addr add 192.168.1.1/24 dev lo
sudo ip addr add 192.168.1.2/24 dev lo
```

#### Plan

Continuously iterating on it since there's so much going on here :)

- [x] Read RFC793 few times
- [x] Boilerplate code
    - [x] Make your own basic socket API calls
    - [x] TCB basic code
- [x] Communication over raw sockets
    - [x] IP Header class
    - [x] Test and verify packet exchange
- [ ] 3 way handshake init
    - [x] Add TCP segment header
    - [x] Basic FSM driver logic (using fn pointers?)
    - [ ] Full state machine
    - [ ] Payload -> TCP Segment + Payload -> IP Header + TCP Segment + Payload
    - [ ] Do 3 way handshake and maintain the state (of FSM) properly
- [ ] Retransmission queue + Timer logic
    - [ ] Test 3 way handshake with this
- [ ] Try sending a packet after connection is established
- [ ] Sequence window logic

End goal: Sending a file as a byte stream over this tcp/ip implementation

#### References
 - https://datatracker.ietf.org/doc/html/rfc791/
 - https://datatracker.ietf.org/doc/html/rfc793
