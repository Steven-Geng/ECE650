#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "player.h"
#include "helperFns.h"

Player::Player(const char * hostname_, const char * hostport_): 
    hostname(hostname_), hostport(hostport_){}

Player::~Player(){
    close(socket_fd_master);
    close(socket_fd_right);
}

void Player::connectToRingMaster(){
    socket_fd_master = connectToServer(hostname, hostport);
}

void Player::recvConnectionInfo(){
    // first, receive current player's ID
    int playerId;
    recv(socket_fd_master, &playerId, sizeof(playerId), 0);

    // then, receive number of players
    int numberOfPlayers;
    recv(socket_fd_master, &numberOfPlayers, sizeof(numberOfPlayers), 0);

    recv(socket_fd_master, &rightPlayerId, sizeof(rightPlayerId), 0);
    recv(socket_fd_master, &leftPlayerId, sizeof(leftPlayerId), 0);

    // then, receive self port number
    //recv(socket_fd_master, &selfPort, sizeof(selfPort), 0);

    // receive right player's port number
    //recv(socket_fd_master, &rightPlayerPort, sizeof(rightPlayerPort), 0);

    // receive right player's IP address
    char buffer[512];
    recv(socket_fd_master, buffer, sizeof(buffer), 0);

    std::cout << "Connected as player " << playerId << " out of "
    << numberOfPlayers << " total players" << std::endl;
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
    std::string rightPortString = std::to_string(rightPlayerPort);
    const char * rightPort = rightPortString.c_str();
    socket_fd_right = connectToServer(rightPlayerIp.c_str(), rightPort);
}

void Player::connectWithLeftPlayer(){
    socket_fd_left = acceptConnection(socket_fd_self);
}

void Player::recvOneInfo(Potato & potato){
    fd_set read_fds;
    FD_ZERO(&read_fds);
    std::vector<int> portsToListen;
    portsToListen.push_back(socket_fd_left);
    portsToListen.push_back(socket_fd_right);
    portsToListen.push_back(socket_fd_master);
    for(int i = 0; i < 3; i++){
        FD_SET(portsToListen[i], &read_fds);
    }
    int numfds = std::max(socket_fd_left, std::max(socket_fd_master, socket_fd_right));
    int status = select(numfds + 1, &read_fds, nullptr, nullptr, nullptr);
    if(status == -1){
        perror("select");
    }
    for(int i = 0; i < 3; i++){
        if(FD_ISSET(portsToListen[i], &read_fds)){
            recv(portsToListen[i], &potato, sizeof(potato), 0);
            break;
        }
    }
}

void Player::doTheGame(Potato & potato){
    while(1){
        recvOneInfo(potato);
        // recv endgame msg from master
        if(potato.getCurrHop() == 0){
            close(socket_fd_left);
            close(socket_fd_master);
            close(socket_fd_right);
            close(socket_fd_self);
            return;
        }
        // recv potato from neighbor
        potato.decreaseHops();
        potato.appendPlayerId(playerId);
        // this player is the last one
        if(potato.getCurrHop() == 0){
            std::cout << "I'm it" << std::endl;
            send(socket_fd_master, &potato, sizeof(potato), 0);
        }
        // continue pass potato
        else{
            int randomNeighbor = generateRandomNumber(2, playerId);
            if(randomNeighbor == 0){
                std::cout << "Sending potato to " << leftPlayerId << std::endl;
                send(socket_fd_left, &potato, sizeof(potato), 0);
            }
            else{
                std::cout << "Sending potato to " << rightPlayerId << std::endl;
                send(socket_fd_right, &potato, sizeof(potato), 0);
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