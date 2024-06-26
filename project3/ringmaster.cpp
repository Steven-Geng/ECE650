#include "ringmaster.h"
#include "helperFns.h"

RingMaster::RingMaster(const char * port_, int numberOfPlayers_, int numberOfHops_):
    numberOfPlayers(numberOfPlayers_), numberOfHops(numberOfHops_), hostname(NULL), port(port_), 
    playerSocketInfo(std::map<int, int>()), playerIpInfo(std::map<int, std::string>()){
        std::cout << "Potato Ringmaster" << std::endl;
        std::cout << "Players = " << numberOfPlayers_ << std::endl;
        std::cout << "Hops = " << numberOfHops_ << std::endl;
    }

RingMaster::~RingMaster(){
    close(socket_fd);
}

void RingMaster::setupMasterServer(){
    socket_fd = setupServer(port, numberOfPlayers);
}

void RingMaster::connectWithPlayers(){
    for(int currId = 0; currId < numberOfPlayers; currId++){
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int playerSocket = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (playerSocket == -1) {
            std::cerr << "Error: cannot accept connection on socket" << std::endl;
            exit(-1);
        }
        std::cout << "Player " << currId << " is ready to play" << std::endl;
        // at first connection, master store the player's ID, IP, Port, and Socket
        playerSocketInfo.insert(std::make_pair(currId, playerSocket));
        // get player ip address
        char playerIpAddr[INET6_ADDRSTRLEN];
	    recv(playerSocket, &playerIpAddr, sizeof(playerIpAddr), 0);
	    std::string playerIp = playerIpAddr;
        playerIpInfo.insert(std::make_pair(currId, playerIp));
        send(playerSocketInfo[currId], &currId, sizeof(currId), 0);//player Id
        send(playerSocketInfo[currId], &numberOfPlayers, sizeof(numberOfPlayers), 0);
	/*
        if(socket_addr.ss_family == AF_INET){
            inet_ntop(socket_addr.ss_family, &(((struct sockaddr_in*)&socket_addr)->sin_addr), playerIpAddr, INET6_ADDRSTRLEN);
        }
        else{
            inet_ntop(socket_addr.ss_family, &(((struct sockaddr_in6*)&socket_addr)->sin6_addr), playerIpAddr, INET6_ADDRSTRLEN);
        }
        std::string playerIp = playerIpAddr;
        playerIpInfo.insert(std::make_pair(currId, playerIp));*/
    }
        
}

void RingMaster::sendConnectionInfoToPlayers(){
    for(int currId = 0; currId < numberOfPlayers; currId++){
        int rightId;
        int leftId;
        if(numberOfPlayers == 2){
            rightId = 1 - currId;
            leftId = 1 - currId;
        }
        else{
            if(currId == numberOfPlayers - 1){
            rightId = 0;
            leftId = numberOfPlayers - 2;
            }
            else if(currId == 0){
                rightId = 1;
                leftId = numberOfPlayers - 1;
            }    
            else{
                rightId = currId + 1;
                leftId = currId - 1;
            }
        }
        //std::cout << "sending ip information: " << playerIpInfo[rightId] << std::endl;
        
        send(playerSocketInfo[currId], &rightId, sizeof(rightId), 0);
        send(playerSocketInfo[currId], &leftId, sizeof(leftId), 0);
        //send(playerSocketInfo[currId], &playerPortInfo[currId], sizeof(playerPortInfo[currId]), 0);
        //send(playerSocketInfo[currId], &playerPortInfo[rightId], sizeof(playerPortInfo[rightId]), 0);
        const char * ipInfo = playerIpInfo[rightId].c_str();
        send(playerSocketInfo[currId], ipInfo, strlen(ipInfo), 0);
    }
}

void RingMaster::recvPlayersServerPort(){
    for(int currId = 0; currId < numberOfPlayers; currId++){
        unsigned int playerServerPort;
        recv(playerSocketInfo[currId], &playerServerPort, sizeof(playerServerPort), 0);
        playerPortInfo.insert(std::make_pair(currId, playerServerPort));
    }
}

void RingMaster::sendServerPortToPlayerNeighbor(){
    for(int currId = 0; currId < numberOfPlayers; currId++){
        int rightId;
        if(currId == numberOfPlayers - 1){
            rightId = 0;
        }
        else{
            rightId = currId + 1;
        }
        //std::cout << "sending port: " << playerPortInfo[rightId] << std::endl;
        send(playerSocketInfo[currId], &playerPortInfo[rightId], sizeof(playerPortInfo[currId]), 0);
    }
}

void RingMaster::sendPotatoToRandomPlayer(Potato & potato){
    srand((unsigned int)time(NULL) + numberOfPlayers);
    int randomId = rand() % numberOfPlayers;
    std::cout << "Ready to start the game, sending potato to player " << randomId << std::endl;
    send(playerSocketInfo[randomId], &potato, sizeof(potato), 0);
}

void RingMaster::waitForPotatoToComeBack(Potato & potato){
    fd_set read_fds;
    FD_ZERO(&read_fds);
    int numfds = 0;
    for(int currId = 0; currId < numberOfPlayers; currId ++){
        FD_SET(playerSocketInfo[currId], &read_fds);
        if(playerSocketInfo[currId] > numfds){
            numfds = playerSocketInfo[currId];
        }
    }
    int status = select(numfds + 1, &read_fds, NULL, NULL, NULL);
    if(status == -1){
        perror("select");
    }
    for(int currId = 0; currId < numberOfPlayers; currId++){
        if(FD_ISSET(playerSocketInfo[currId], &read_fds)){
            recv(playerSocketInfo[currId], &potato, sizeof(potato), MSG_WAITALL);
            break;
        }
    }
    
}

void RingMaster::endTheGame(Potato & potato){
    for(int currId = 0; currId < numberOfPlayers; currId++){
        send(playerSocketInfo[currId], &potato, sizeof(potato), 0);
    }
    const int * trace = potato.getPlayerTrace();
    std::cout << "Trace of potato:" << std::endl;
    if(numberOfHops != 0){
        std::cout << trace[0];
        for(int i = 1; i < potato.getCurrHop(); i++){
            std::cout << "," << trace[i];
        }
    }
    std::cout << std::endl;
    for(int currId = 0; currId < numberOfPlayers; currId++){
        close(playerSocketInfo[currId]);
    }
}

int main(int argc, char ** argv){
    if(argc != 4){
        std::cerr << "Usage: ./ringmaster <port_num> <num_players> <num_hops>" << std::endl;
        std::cerr << "For example: ./ringmaster 1234 3 100" << std::endl;
        exit(-1);
    }
    int numberOfPlayers = std::atoi(argv[2]);
    int numberofHops = std::atoi(argv[3]);
    if(numberOfPlayers <= 1){
        std::cerr << "The number of players must be greater than 1." << std::endl;
        exit(-1);
    }
    if(numberofHops < 0 || numberofHops > 512){
        std::cerr << "The number of hops must be greater than or equal to zero and less than or equal to 512." << std::endl;
        exit(-1);
    }
    RingMaster master(argv[1], numberOfPlayers, numberofHops);
    master.setupMasterServer();
    master.connectWithPlayers();
    master.sendConnectionInfoToPlayers();
    master.recvPlayersServerPort();
    master.sendServerPortToPlayerNeighbor();
    Potato potato(numberofHops);
    master.sendPotatoToRandomPlayer(potato);
    master.waitForPotatoToComeBack(potato);
    master.endTheGame(potato);
    return 0;
}
