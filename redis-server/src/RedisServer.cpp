#include "../include/RedisServer.h"
#include "../include/RedisCommandHandler.h"
#include <vector>
#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>

static RedisServer* globalServer = nullptr;

RedisServer::RedisServer(int port) : port(port), server_socket(-1), running(true) {
    globalServer = this;
}

void RedisServer::shutdown() {
    running = false;
    if (server_socket != -1) {
        close(server_socket);
    }
    std::cout << "***THE SERVER IS SHUTDOWN***" << std::endl;
}

void RedisServer::run() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating the server socket :(" << std::endl;
        return;
    }

    // Fix: Use 1 for SO_REUSEADDR
    int options = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding server socket" << std::endl;
        return;
    }

    if (listen(server_socket, 10) < 0) {
        std::cerr << "Error listening" << std::endl;
        return;
    }

    std::cout << "The Redis Server is listening on: " << port << std::endl;

    std::vector<std::thread> threads;
    RedisCommandHandler commandHandler;  // Warning: Shared across threads (needs mutex)

    while (running) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            if (running) {
                std::cerr << "Error accepting client connection" << std::endl;
            }
            continue;  // Skip to next iteration
        }

        // Thread per client
        threads.emplace_back([client_socket, &commandHandler]() {
            char buffer[1024];
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0) break;
                std::string request(buffer, bytes);
                std::string response = commandHandler.processCommand(request);
                send(client_socket, response.c_str(), response.size(), 0);
            }
            close(client_socket);
        });
    }

    // Join all threads
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}
