#include "../include/RedisServer.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

static RedisServer* globalServer = nullptr;

RedisServer::RedisServer(int port):port(port), server_socket(-1), running(true){
    globalServer = this;
}

void RedisServer::shutdown() {
    running = false;
    if(server_socket != -1){
        close(server_socket);
    }

    std:: cout << "***THE SERVER IS SHUTDOWN***" << std::endl;
}


void RedisServer::run(){
    server_socket = socket(AF_INET, SOCK_STREAM,0);
    if( server_socket < 0) {
        std::cerr << "Error creating the server socker :(" << std::endl;
        return;
    }

    int options = -1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding server socket " << std::endl;
        return;
    }

    if(listen(server_socket, 10) < 0) {
        std:: cerr << "Error :(" << std::endl;
        return;
    }

    std:: cout << "The Redis Server is listening on :" << port << std:: endl;

}
