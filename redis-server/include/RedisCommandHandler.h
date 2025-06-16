#ifndef REDIS_COMMAND_HANDLER_H
#define REDIS_COMMAND_HANDLER_H

#include <string>

class RedisCommandHandler {
    public:
    RedisCommandHandler();

   //Takes in a command from the client, processes it and returns a RESP-formatted response
    std::string processCommand(const std::string& commandLine);
};

#endif
