# Ciopillis
Stefan Petrovici --- 2019-2020

Introduction
-------------------
Ciopillis is a video game project that has been developed, at first, by the author as a learning exercise for C++ concepts and afterward extended for acquiring knowledge related to network programming. The project was submitted for evaluation as part of the 2019-2020 Computer Networks course of the A.I. Cuza University.


Used technologies
-------------------

### ECS

The game was built with the use of an [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system) framework, followingly referenced as ECSlib.

To understand the ECS design better we compare it to what was considered the natural way of implementing a game system: **object-oriented inheritance hierarchies**. Everything in the game was an instance of the class **Object** or its derived classes. Each object would interact most commonly with either it's base or derived classes instantiations, and in some cases with other classes. **The game logic** was based on the **coordination between the classes**, most often in the form of *hierarchical trees*, with parents controlling the actions of the children and the children transmitting responses.

But at a certain point, several problems arose. One of the most important was the process of maintaining the code: Adding new functionalities inevitably involved the use of increasingly complex issue-solving techniques, which even though was educational, proved tedious and hindered the development of the game.

With ECS, this aspect is solved:

> **Entity–component–system** (**ECS**) is an architectural pattern that is mostly used in the game development industry. ECS follows the composition over the inheritance principle that allows greater flexibility in defining game object types. The behavior of each entity can change depending on what components it holds at a certain moment.

 **Entities** (the correspondent of Objects) now held attributes called **Components** (essentially data units), that are evaluated independently in such a matter that these structures are seemingly dependency-free. **The new game logic** is driven by **Systems**, each taking a specific type of Component and editing the data in these units. Adding new functionalities now consists of adding a new System with an independent logic and a matching component (though this is not mandatory). Also see the [Appendix ](#Appendix ).

Because these systems are independent, the adding, removing, runtime disabling or enabling would not crash the game or impact in any way other modular functionalities. Also, the behavior of the main game unit, the Entity, can easily change during the game based on the contained components.

### The UDP protocol

Choosing a method for network handling considered the following aspects of the game:
- the game is multiplayer, so the server must accept multiple connections at the same time
- when two clients want to start a game, they enter a separate room, and they start sharing packets through the server
- the game has a real-time interaction between the players in the arena, it is imperative that the connection appears as smooth and accurate as possible
-  the game has a part where players take turns, in those instances, each client must wait for a response

#### Implementation

The most commonly used protocols for sending packets over the internet are TCP and UDP. Between the two protocols, the most useful choice would be UDP, but there are some trade-offs. UDP is also mostly used for network games, and there is a reason for that, as we will see in the following.

##### UDP - Pros:

The most obvious advantage of UDP is the speed for sending and receiving the data. The keys (UP, DOWN, LEFT, RIGHT, A, B) that the players press must be sent to the server rapidly, such that the response that the sending client, but also the other client sees is viewed  almost instantly. This will allow the players to make the decisions faster and be entertained by competing against each other.
If we had chosen TCP we would have noticed a decrease in the speed of the connection, created by the validations that TCP does to ensure that the packets are received fully and in order. Furthermore, there are instances when the packets are lost and TCP waits and resends those packets, but upon resuming the flow, the connection speeds up to compensate for the delay. This would feel rushed to the client and the gameplay would then appear to stutter.
The size of the packets is also favorable for this protocol, as UDP is more optimized for this case.

##### UDP - Cons

The trade-off for this method is that we need to account for the out of sync scenarios between the clients' current game states. This could happen when one client moves one direction and the packet that specifies this is lost. The client's graphical interface would show that the player has moved, but the server did not convey that to the other user. This player would then be led to believe that the first user's location would be in the wrong place, and direct his attack to that area.  This will be, however, resolved by the server, as it will sync the two players coordinates at the next packet transmission.

The packets could also be received out of order or become corrupted, but in this instances, by looking at the timestamp/content hash of the packets and only accepting the latest ones or the valid ones respectively, we can solve this issue.

### Raylib

Raylib is an open-source, cross-platform library for video game written in c99 that is used by the ECSlib for rendering and physics computations.

Architecture
-------------------
### The framework architecture

A game instance is usually managed by an object of the class **ECSManager**. ECSManager ties together a **SystemManager**, an **Event Manager** and a **Resource Manager**. Each system can be initilalized and runs idependently, but can sometimes communicate with other systems by **Events**.

Just to mention a few important systems, ECSlib implements the **DrawSystem** (graphical manager), **InputSystem**, **AnimationSystem**, **PhysicsSystem** etc. Each system can run on a separate thread, however the current implementation works with an iterative approach to the concept.

### The game diagram

![Game Diagram](https://drive.google.com/uc?export=download&id=15BQs6OjsTcAlnQFBJxINkpDQAUUSfVCn)



The main transitions are shown in the picture above. Rendering is done by the DrawSystem, by calling functions from the Raylib library.

### The network architecture

![NetDiagram](https://drive.google.com/uc?export=download&id=1-K77OPiY3wnoQsDSSIoxDDIzspBownFK)
Upon running the client, a socket is created and the request for a game room is sent to the server. This is then registered in a queue, called Game Start Queue or Lobby. When the server notices two client requests, it creates a game room and the sends the game start prompt. The concept of game room is currently implemented not as a peer-to-peer connection between the two clients, the communication is done through the server. Though implementing p2p would be an improvement, the current method allows for the server to more easily inspect or control the flow between the players. This could allow for potential future feature implementations (ex, server streaming the game room to viewers outside the game). When the game ends, the users are asked if they want to rejoin the Lobby.

Implementation
-------------------
### ECSlib: The Systems

Implementation for each of the systems is complex to start describing and it is neither the purpose for this paper. Therefore it will be discussed at a later date.

### ECSlib: The NetworkSystem class

The network system is a singleton class object per program instance, and has different functionalities depending on the type of the user (client/server). Upon initialization, in the case of servers, it readies a communcation thread and then becomes able to create  other such threads. The clients start by attempting to communicate the first message of a predefined transfer sequence between the client and the server.

**Server:**
```c
CHECK(sd = socket (AF_INET, SOCK_DGRAM, 0));

bzero (&server, sizeof (server));
bzero (&client, sizeof (client));

server.sin_family = AF_INET;
server.sin_addr.s_addr = htonl (INADDR_ANY);
server.sin_port = htons (PORT);

CHECK(bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)));

//starting a communication thread
THREAD_CREATE {
   CHECK(recvfrom(sd, msg, 100, 0,(struct sockaddr*) &client, &length)));
}
```
**Client:**
```c
CHECK(sd = socket (AF_INET, SOCK_DGRAM, 0));

bzero (&server, sizeof (server));

server.sin_family = AF_INET;
server.sin_addr.s_addr = htonl (IP_ADDRESS);
server.sin_port = htons (PORT);

CHECK(sendto(sd, msg, 100, 0,(struct sockaddr*) &server, &length)));
```

 **The transfer sequence**
 The format of all the messages in the networking communication are `.json`. A certain message from the client starts the entire networking process, and contains some information:
```js
{
  "head":"start_all",
  "mode": "remote", // or localhost
  "username": "some_usr",
  "password": 0, //not yet implemented
  "hash" : "ad5ms9ff2"
}
```
After two players are connected, the server might send back a reply json with information about the initiated match.
```js
{
  "head":"start_game",
  "other_username": "the_other_usr",
  "hash" : "ad5ms9ff2"
}
```
In the **card selection phase** the users take turns to perform an action, and send the input. The input is captured from the user by the **InputSystem** and passed as an **Event** to the **CardGameEventSystem**. The input is validated and sent again as an event to the **NetworkSystem**.
```js
{
  "head":"card_selection_turn",
  "action": "draw", // can be "discard", "play_card", "skip"
  "error": 0, //0 if none, could be 1-timeout, 2-close etc.
  "hash" : "ad5ms9ff2"
  ... //auxiliary fields, depending on the command
}
```
The server responds with statuses:
```js
 {
  "head":"card_selection_response",
  "status": "valid_choice",
  "error": 0,
  "hash" : "ad5ms9ff2"
  ...
}
```
When the response is sent, this system will notify through the same procedure the event system. If both players have chosen an action the response of the server is sent back to them and the game proceeds to **arena phase**. In this phase players can send their current coordinates to the server and the input actions:
```js
 {
  "head":"arena_action",
  "action": "attack",
  "coordinates": [100, 200],
  "pressed": "LEFT, X, Y",
  "error": 0,
  "hash" : "ad5ms9ff2"
  ...
}
```
The Arena interacts in the same way with its own event system called ArenaGameEventSystem. The server response could then be:
```js
 {
  "head":"arena_action_response",
  "coordinates_self": [99, 199], //will overwrite self coordinates,
  "hp_self": 100,
  "coordinates_opponent": [120, 400],
  "hp_opponent": 40,
  "error": 0,
  "hash" : "ad5ms9ff2"
  ...
}
```
The coordinates of the players are then overwritten at every exchange.
One of the terminating messages would specify the winner or an error:
```js
 {
  "head":"fatal_error",
  "coordinates_self": [-100, 0], //invalid coord, corrupt data
  "hp_self": 1,
  "coordinates_opponent": [120, 400],
  "hp_opponent": 50,
  "error": 0,
  "hash" : "ad5ms9ff2"
  ...
}
```
In case of an error the client will automatically close, yet the server will continue to function normally.

Conclusions
-------------------
Depending on what we favor, the network system may be improved by making the connection between the clients peer-to-peer. If this was to be implemented, the transmission of packets would be faster. A scenario handling for the case where the server stops working and the clients are already in a game must be created.

Another improvement would be about the sync between the transmitted player coordinates and the graphical interface. In case of desync, instead of sending/receiving our current position (s => p1 coord. r <= p2 coord.),overwriting the outdated player coordinates when the connection is re-established, we can obtain a more reliable set of data if we first send our current inputs to the server (s => key.RIGHT) and the server transmits back a tuple of (r <= p1 coord., r <= p2 coord.).

We can also note that the above problem has another well-known solution in the game development industry: input prediction. With the use of adaptive algorithms, the disconnect time can be compensated by making small computer generated inputs that make the connection seemingly uninterrupted.

References
---
[Computer & Networks course](https://profs.info.uaic.ro/~computernetworks/cursullaboratorul.php)

[Raylib](https://www.raylib.com/)

[Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system)

[TCP and UDP reference](https://profs.info.uaic.ro/~computernetworks/files/4rc_NivelulTransport_En.pdf)

[TCP and UDP pros and cons](https://www.privateinternetaccess.com/blog/2018/12/tcp-vs-udp-understanding-the-difference/)

[Github repo of the project](https://github.com/firststef/Ciopillis)

# Appendix

How ECS Systems work
![Systems](https://drive.google.com/uc?export=download&id=1ONHaTytjKBFrBXvTDf1QOMA3MaFs5X4O)

*Each System only changes data by small, modular logic rules without interfering too much with other system's logic*.  
