# cpp-tcpip

This project is a custom implementation of the TCP/IP protocol stack (as per RFC 793), simulating core TCP mechanisms such as the 3-way handshake and basic data transmission. It includes a user-space simulation where socket operations are handled by communicating with a kernel-like thread, which manages connection states and packet transmission.

## Features
 - **3-Way Handshake**: The implementation successfully establishes a TCP connection using the SYN, SYN-ACK, and ACK packets.
 - **Data Transmission**: Sending and receiving data between connected sockets is functional (currently using some workarounds).
 - **User and Kernel Thread Simulation**: Socket operations are initiated by the user thread and executed by a separate "kernel" thread using message queues for inter-thread communication.

## Current Status

Core mechanism of the TCP protocol are implemented (with some hacks) including connection establishment (3-way handshake) and data transfer. Details are mentioned below in **Plan** section.

## Usage 

Note: Currently, our 3-way handshake doesn't support timeout + retransmission so first run `myserver`, wait for it to reach `LISTEN` state and then run `myclient` to reach `ESTABLISHED` state.

1. Clone the repository

2. Add the two ips in lo loopback interface

```
sudo ip addr add 192.168.1.1/24 dev lo
sudo ip addr add 192.168.1.2/24 dev lo
```

3. Build the project

```
make -j4
```

4. Open two terminals and run below:

```
sudo ./myserver |tee server.log
```

and

```
sudo ./myclient |tee client.log
```

## Plan

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
    - [x] Redesign the socket design for 'userland' and 'tcp' (kernel) thread state
    - [x] Communicate
    - [x] Move socket logic to new thread
    - [x] Implement timer logic
    - [x] Stop the tcp thread gracefully
    - [x] Verify the handshake that worked before
    - [x] Stop threads once handshake is done for now
- [x] Proper handshake
    - [x] Fix timer. Timeout should fail and after receiving ACK, delete timer
    - [x] Implement it as designed in the "Event Processing" chapter in RFC
    - [?] Full state machine
    - [x] Do 3 way handshake and maintain the state (of FSM) properly
- [x] Close socket and free resources
- [x] Send and receive data
    - [x] Insert packet in a buffer and use a bitset with mutex
    - [x] Retrieve packet in tcp thread
    - [x] Add tcp/ip headers and send to socket
    - [x] After ESTABLISHED, server needs to listen for data
    - [x] For now, get the packet and dump it
- [x] Add debug logging
    - [x] write to a logfile to keep it separate from user code
- [ ] Sequence and ACK logic
- [ ] Revisit the ACTION logic of FSM (should be more tightly coupled)
- [ ] Revisit FSM hacking done in recvSegment
- [ ] Retransmission queue
    - [ ] Test 3 way handshake with this
- [ ] Add properly error codes
- [ ] getISN from time module 2**32

End goal: Sending a file as a byte stream over this tcp/ip implementation

## Gochas and lessons learned

 - Assigned a char* pointer from MyMsg into a packet but the MyMsg instance gets deleted so pointer now pointed to undefined area.
 - Assigned and mutated a local copy of an object and then was confused why the actual object wasn't updated.
 - Called join of the second thread in it's own context (instead of main thread) causing an exception

## References
 - https://datatracker.ietf.org/doc/html/rfc791/
 - https://datatracker.ietf.org/doc/html/rfc793
