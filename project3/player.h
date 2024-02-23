#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "potato.h"

class Player{
private:
    int playerId;
    int rightPlayerId;
    int leftPlayerId;
    int socket_fd_master;
    int socket_fd_right;
    int socket_fd_self;
    int socket_fd_left;
    const char * hostname;
    const char * hostport;
    unsigned int selfPort;
    unsigned int rightPlayerPort;
    std::string rightPlayerIp;

public:
    Player(const char * hostname_, const char * port_);
    ~Player();
    void connectToRingMaster();
    void recvConnectionInfo();
    void setupServerForLeftPlayer();
    void sendSelfPortInfoToMaster();
    void recvRightNeighborPortInfo();
    void connectToRightPlayer();
    void connectWithLeftPlayer();
    void recvOneInfo(Potato & potato);
    void doTheGame(Potato & potato);
};