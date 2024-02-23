#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <map>
#include <arpa/inet.h>
#include "potato.h"

class RingMaster{
private:
    int numberOfPlayers;
    int numberOfHops;
    int socket_fd;
    const char * hostname;
    const char * port;
    std::map<int, int> playerSocketInfo; // id socket
    std::map<int, std::string> playerIpInfo; // id IP
    std::map<int, unsigned int> playerPortInfo; // id portnum

public:
    RingMaster(const char * port_, int numberOfPlayers_, int numberOfHops_);
    ~RingMaster();
    void setupMasterServer();
    void connectWithPlayers();
    void sendConnectionInfoToPlayers();
    void recvPlayersServerPort();
    void sendServerPortToPlayerNeighbor();
    void sendPotatoToRandomPlayer(Potato & potato);
    void waitForPotatoToComeBack(Potato & potato);
    void endTheGame(Potato & potato);

};