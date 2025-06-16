#ifndef REDIS_SERVER_H
#define REDIS_SERVER_H

#include <string>
#include <atomic>

class RedisServer {
public:
    RedisServer(int port); // Constructor declaration
    void run();
    void shutdown();

private:
    int port; // Port number
    int server_socket; // Socket
    std::atomic<bool> running; // Check for Socket runnning
};

#endif // REDIS_SERVER_H
