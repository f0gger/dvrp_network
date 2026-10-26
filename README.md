# DVRP Network

A multi-threaded UDP networking project implemented in C, featuring a client-server messaging system and a distance-vector-style routing mechanism for forwarding messages between routers.

The project separates message delivery into two protocols:

    TP (Transport Protocol): Handles client messages, inbox registration, and message delivery.
    RP (Routing Protocol): Handles router discovery, routing table maintenance, and forwarding packets between routers.

## Features

    UDP-based communication using POSIX sockets.
    Multi-threaded client and server implementations using POSIX threads.
    Message sending and inbox management.
    Client registration and periodic heartbeat messages.
    Router discovery through periodic broadcasts.
    Routing tables containing destination, next hop, hop count, and last-seen timestamp.
    Periodic cleanup of inactive clients and stale routing entries.
    Thread synchronization using mutexes and semaphores.
    Periodic timers implemented with Linux timerfd.
    Makefile-based compilation.

## Repository Structure

```text

dvrp_network/
├── include/
│   ├── comm_defs.h
│   ├── err_codes.h
│   ├── lists.h
│   ├── packet_types.h
│   ├── timerfd.h
│   └── tools.h
├── lib/
│   ├── lists.c
│   ├── timerfd.c
│   └── tools.c
├── src/
│   ├── client.c
│   └── server.c
├── Makefile
└── README.md
```

## Requirements

    Linux operating system.
    GCC or another compatible C compiler.
    GNU Make.
    POSIX threads and semaphores.
    Linux timerfd support.

## Building

Clone the repository and enter the project directory:
bash

git clone <repository-url>
cd dvrp_network

Compile both the client and server:

```bash
make
```

The executables are generated in the bin/ directory:

```text

bin/
├── client
└── server
```

The Makefile also creates a build/ directory for intermediate object files.

To remove the generated executables:
```bash

make clean
```

## Running

The server and client use UDP sockets and currently have their IP address and port settings defined in the source code.
1. Start the server

Run the server on each machine intended to act as a router:
```bash

./bin/server
```

The server prompts for its actual IP address and the IP addresses of its neighboring routers.

Enter the reachable IP address of the local machine, followed by the addresses of its configured neighbors. The current implementation supports up to two neighboring routers.
2. Start a client

Run a client on a machine that can reach the server:
```bash

./bin/client
```

The client prompts for an IP address used to derive its logical network address.

Once started, the client presents an interactive menu for sending messages and managing its inbox.

## Client Commands

The client provides the following menu:
| Command | Description |
| --- | --- |
| `1` | Send a message |
| `2` | Open inbox |
| `3` | Close inbox |
| `4` | Check inbox |
| `5` | Exit |

Destination addresses use the logical format rrr.hhh, where the first three digits identify the router and the last three identify the host.

For example:
```text

001.002
```

Messages have a maximum defined payload size of 64 characters.

## Network Architecture
### Transport Protocol (TP)

The transport protocol handles client-to-server communication and message delivery.

Its responsibilities include:

    Registering clients when they open their inbox.
    Removing clients when they close their inbox or become inactive.
    Receiving client messages and checking their destination.
    Delivering messages to locally registered clients.
    Forwarding messages to the routing protocol when the destination belongs to another router.
    Returning status responses to clients.

### Routing Protocol (RP)

The routing protocol handles communication between routers.

Its responsibilities include:

    Periodically broadcasting router information to neighboring routers.
    Maintaining a routing table with destination routers, next hops, hop counts, and timestamps.
    Forwarding packets toward their final destination.
    Refreshing routing information when broadcasts are received.
    Removing stale routing entries.

The routing implementation follows a distance-vector-style approach, using hop counts and next-hop information to make forwarding decisions.

## Packet Types

Packet structures are defined in include/packet_types.h.

### TP_Packet

Represents a client message and contains:

    dest — logical destination address.
    src — logical source address.
    cmd — command identifying the requested operation.
    msg — message payload.

### RP_Packet

Wraps a transport packet for communication between routers and contains:

    final_dest — destination router.
    curr_src — current or forwarding router.
    start_src — originating router.
    cmd — routing operation.
    hop_cnt — hop count.
    tp_pkt — encapsulated transport packet.

## Project Organization

    include/ — Shared definitions, packet structures, error codes, and function declarations.
    lib/ — Shared linked-list, timer, and networking utility implementations.
    src/client.c — Interactive client, message sending, inbox handling, and heartbeat generation.
    src/server.c — Server-side message processing, routing, packet forwarding, and router discovery.
    Makefile — Build rules for the client and server executables.

## Configuration

The current network configuration is defined using constants in the source files.

| Setting | Default value | Description |
| --- | --- | --- |
| Server IP | `127.0.0.1` | Default client-server destination address |
| Server port | `5001` | UDP port used for client-server communication |
| Router port | `7001` | UDP port used for router-to-router communication |
| Client starting port | `5002` | Starting port for client socket binding |
| Router neighbor limit | `2` | Maximum number of configured neighboring routers |

**Note:** The current configuration uses 127.0.0.1 for client-to-server communication, so clients and servers must run on the same machine unless the source configuration is changed. Router communication uses configured network addresses.
