#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <map>
#include <arpa/inet.h>
#include <ctime>
#include <cstdio>

int setupServer(const char * port, int numberOfPlayers){
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo * host_info_list;
    int status;

    memset(& host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;
    const char * hostname = INADDR_ANY;

    status = getaddrinfo(hostname, port, & host_info, & host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(-1);
    }

    socket_fd = socket(host_info_list -> ai_family,
                    host_info_list -> ai_socktype,
                    host_info_list -> ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        exit(-1);
    }

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot bind socket" << std::endl;
        exit(-1);
    }

    status = listen(socket_fd, numberOfPlayers);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket" << std::endl;
        exit(-1);
    }

    freeaddrinfo(host_info_list);

    return socket_fd;
}

unsigned int getPortNumberFromSocket(int socket_fd){
    struct sockaddr_in socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    if (getsockname(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len) == -1){
        perror("getsockname");
    }
    else{
        return ntohs(socket_addr.sin_port);
    }
    return 0;
}

unsigned int connectToServer(const char * hostname, const char * port){
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo * host_info_list;

    memset(& host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    int status;

    status = getaddrinfo(hostname, port, & host_info, & host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(-1);
    }

    socket_fd = socket(host_info_list -> ai_family,
                    host_info_list -> ai_socktype,
                    host_info_list -> ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(-1);
    }

    //std::cout << "Connecting to " << hostname << " on port " << port << "..." << std::endl;

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(-1);
    }

    freeaddrinfo(host_info_list);
    return socket_fd;
}

unsigned int acceptConnection(unsigned int socket_fd_self){
    int socket_fd;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    socket_fd = accept(socket_fd_self, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot accept connection on socket" << std::endl;
        exit(-1);
    }
    return socket_fd;
}

int generateRandomNumber(int upperBound, int uniqueVar){
    srand((unsigned int)time(NULL) + uniqueVar);
    return rand() % upperBound;
}

