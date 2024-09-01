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
- [x] 3 way handshake init
    - [x] Add TCP segment header
    - [x] Basic FSM driver logic (using fn pointers?)
    - [x] One way FSM
    - [x] Payload -> TCP Segment + Payload -> IP Header + TCP Segment + Payload
- [x] Reorganize code before it gets out of hand
- [x] Make a different thread for TCP stuff
    - [x] Redesign the socket design for 'userland' and 'kernel' thread state
    - [x] Communicate
    - [x] Move socket logic to new thread
    - [x] Implement timer logic
    - [x] Stop the kernel thread gracefully
    - [x] Verify the handshake that worked before
    - [x] Stop threads once handshake is done for now
- [x] Proper handshake
    - [x] Fix timer. Timeout should fail and after receiving ACK, delete timer
    - [x] Implement it as designed in the "Event Processing" chapter in RFC
    - [?] Full state machine
    - [x] Do 3 way handshake and maintain the state (of FSM) properly
- [ ] Close socket and free resources
- [ ] Revisit the ACTION logic of FSM (should be more tightly coupled)
- [ ] Send data after handshake and then close socket
- [ ] Retransmission queue
    - [ ] Test 3 way handshake with this
- [ ] Sequence window logic
- [ ] getISN from time module 2**32

End goal: Sending a file as a byte stream over this tcp/ip implementation

#### Stupid mistakes I did

 - Assigned a char* pointer from MyMsg into a packet but the MyMsg instance gets deleted so pointer now pointed to undefined area.
 - Assigned and mutated a local copy of an object and then was confused why the actual object wasn't updated.
 - Called join of the second thread in it's own context (instead of main thread) causing an exception

#### References
 - https://datatracker.ietf.org/doc/html/rfc791/
 - https://datatracker.ietf.org/doc/html/rfc793
