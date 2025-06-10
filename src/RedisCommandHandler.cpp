#include "../include/RedisCommandHandler.h"
#include <cctype>
#include <vector>
#include <sstream>
#include <algorithm>

//Redis uses the RESP protocol for redis client server communication
// There are two types of parsing strings in redis => simple strings, bulk strings
// Commands are typically sent as arrays of bulk strings
// Redis uses CRLF (\r\n) as a delimiter between protocol elements.
//RESP parser:
//  *2\r\n$4\r\n\PING\r\n$4\r\nTEST\r\n
// *2 - array with 2 elements
// $4 - bulk string of 4 characters
// PING - the actual string
// $4 - another bulk string
// TEST - second string
// \r\n => CRLF protocol's terminator , which always separates it's parts
//
std::vector<std::string> parseRespCommand(const std::string &input) {
    std::vector<std::string> tokens;

    if(input.empty()) return tokens;

    if(input[0] != '*') {   // means the string falls in simple space-separated parsinf
        std::istringstream iss(input);
        std::string token;

        while(iss >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

    size_t position = 0;

    //Expect '*' followed by number of elements
    if(input[position] != '*') return tokens;
    position++;

    //crlf -> Carriage return(\r), Line Feed (\n)

    size_t crlf = input.find("\r\n", position);
    if(crlf == std:: string::npos) return tokens;


    int numElemnets = std::stoi(input.substr(position, crlf - position));
    position = crlf + 2;

    for(int i=0; i<numElemnets; i++) {
        if(position >= input.size() || input[position] != '$') {
            break;
        }

        position++;

        crlf = input.find("\r\n", position);

        if(crlf == std::string::npos) break;

        int len = std::stoi(input.substr(position, crlf - position));

        position = crlf + 2;


        if(position + len > input.size()) break;

        std::string token = input.substr(position, len);
        tokens.push_back(token);
        position = position + len + 2;
    }

    return tokens;
}

RedisCommandHandler::RedisCommandHandler() {}

std::string RedisCommandHandler::processCommand(const std::string& commandLine) {
    auto tokens = parseRespCommand(commandLine);

    if(tokens.empty()){

        return "Error: empty command \r\n";
    }

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), :: toupper);

    std::ostringstream response;

    return response.str();
}
