#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <sstream>
#include "player.h"
#include "helperFns.h"

Player::Player(const char * hostname_, const char * hostport_): 
    hostname(hostname_), hostport(hostport_){}

Player::~Player(){
    close(socket_fd_master);
    close(socket_fd_right);
    close(socket_fd_left);
    close(socket_fd_self);
}

void Player::connectToRingMaster(){
    socket_fd_master = connectToServer(hostname, hostport);
    char hostname_[256];
    char * playerIpAddr;
    memset(&hostname_, 0, sizeof(hostname_));
    gethostname(hostname_, sizeof(hostname_));
    struct hostent* hostInfo = gethostbyname(hostname_);
    if(hostInfo != NULL){
      struct in_addr address;
      memcpy(&address, hostInfo->h_addr_list[0], sizeof(struct in_addr));
      playerIpAddr = inet_ntoa(address);
      send(socket_fd_master, playerIpAddr, strlen(playerIpAddr)+1, 0);
    }
    // first, receive current player's ID
    recv(socket_fd_master, &playerId, sizeof(playerId), 0);
    // then, receive number of players
    int numberOfPlayers;
    recv(socket_fd_master, &numberOfPlayers, sizeof(numberOfPlayers), 0);
    std::cout << "Connected as player " << playerId << " out of "
    << numberOfPlayers << " total players" << std::endl;
}

void Player::recvConnectionInfo(){
    recv(socket_fd_master, &rightPlayerId, sizeof(rightPlayerId), 0);
    recv(socket_fd_master, &leftPlayerId, sizeof(leftPlayerId), 0);
    // receive right player's IP address
    char buffer[100];
    recv(socket_fd_master, buffer, sizeof(buffer), 0);
    rightPlayerIp = buffer;
}

void Player::setupServerForLeftPlayer(){
    socket_fd_self = setupServer("0", 1);
}

void Player::sendSelfPortInfoToMaster(){
    unsigned int myServerPort = getPortNumberFromSocket(socket_fd_self);
    //std::cout << "My server port: " << myServerPort << std::endl;
    send(socket_fd_master, &myServerPort, sizeof(myServerPort), 0);
}

void Player::recvRightNeighborPortInfo(){
    recv(socket_fd_master, &rightPlayerPort, sizeof(rightPlayerPort), 0);
    //std::cout << "right port: " << rightPlayerPort << std::endl;
}

void Player::connectToRightPlayer(){
  std::stringstream ss;
  ss << rightPlayerPort;
  std::string rightPortString = ss.str();
  const char * rightPort = rightPortString.c_str();
  socket_fd_right = connectToServer(rightPlayerIp.c_str(), rightPort);
}

void Player::connectWithLeftPlayer(){
    socket_fd_left = acceptConnection(socket_fd_self);
}

void Player::recvOneInfo(Potato & potato){
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(socket_fd_master, &read_fds);
    FD_SET(socket_fd_left, &read_fds);
    FD_SET(socket_fd_right, &read_fds);
    
    int numfds = std::max(socket_fd_left, std::max(socket_fd_master, socket_fd_right));
    int status = select(numfds + 1, &read_fds, NULL, NULL, NULL);
    if(status == -1){
        perror("select");
    }
    if(FD_ISSET(socket_fd_master, &read_fds)){
        recv(socket_fd_master, &potato, sizeof(potato), MSG_WAITALL);
    }
    else if(FD_ISSET(socket_fd_left, &read_fds)){
        recv(socket_fd_left, &potato, sizeof(potato), MSG_WAITALL);
    }
    else if(FD_ISSET(socket_fd_right, &read_fds)){
        recv(socket_fd_right, &potato, sizeof(potato), MSG_WAITALL);
    }
}

void Player::doTheGame(Potato & potato){
    srand((unsigned int)time(NULL) + playerId);
    while(true){
        recvOneInfo(potato);
        // recv endgame msg from master
        if(potato.getRemainingHop() == 0){
	        return;
        }
        // recv potato from neighbor
        //std::cout << "remaining after decreasing: " << potato.getRemainingHop() << std::endl;
        potato.decreaseHops();
        potato.appendPlayerId(playerId);
        potato.increaseCurrHop();
        //std::cout << "remaining hops: " << potato.getRemainingHop() << std::endl;
        // this player is the last one
        if(potato.getRemainingHop() == 0){
            std::cout << "I'm it" << std::endl;
            send(socket_fd_master, &potato, sizeof(potato), 0);
            continue;
        }
        // continue pass potato
        else{
            int randomNeighbor = rand() % 2;
            //std::cout << "neighbor: " << randomNeighbor << std::endl;
            if(randomNeighbor == 0){
                std::cout << "Sending potato to " << leftPlayerId << std::endl;
                send(socket_fd_left, &potato, sizeof(potato), 0);
                continue;
            }
            else{
                std::cout << "Sending potato to " << rightPlayerId << std::endl;
                send(socket_fd_right, &potato, sizeof(potato), 0);
                continue;
            }
        }
    }
}


int main(int argc, char ** argv){
    if(argc != 3){
        std::cerr << "Usage: ./player <machine_name> <port_num>" << std::endl;
        std::cerr << "For example: ./player vcm-38071.vm.duke.edu 1234" << std::endl;
    }

    Player player(argv[1], argv[2]);
    player.connectToRingMaster();
    player.recvConnectionInfo();
    player.setupServerForLeftPlayer();
    player.sendSelfPortInfoToMaster();
    player.recvRightNeighborPortInfo();
    player.connectToRightPlayer();
    player.connectWithLeftPlayer();
    Potato potato(0);
    player.doTheGame(potato);
    return 0;
}
