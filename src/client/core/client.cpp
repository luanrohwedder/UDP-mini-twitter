#include "client.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>

Client::Client(const std::string &username, const std::string &ip, int port)
    : _username(username), _running(false)
{
    if ((_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        error("Failed to create socket");

    memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    _serverAddr.sin_port = htons(port);
}

Client::~Client()
{
    _running = false;
    close(_sockfd);
}

int Client::connectToServer()
{
    setTimeout(TIMEOUT_TIME);
    
    Message message(Message::OI, _id, 0, _username, "");
    message.send(_sockfd, _serverAddr);

    Message* response = Message::receive(_sockfd, _serverAddr, BUFFER_SIZE);

    if (response)
    {
        if (response->getType() == 0)
        {
            _id = response->getDestinationID();
            _running = true;
            std::cout << "Connected to server with ID: " << _id << std::endl;
        }

        delete response;
        setTimeout();

        return 1;
    }
    else
    {
        return 0;
    }
}

void Client::sendMessage(const std::string &msg, Message::MessageType messageType, int destinationID)
{
    Message message(messageType, _id, destinationID, _username, msg);
    message.send(_sockfd, _serverAddr);
}

Message* Client::receiveMessages()
{
    Message* msg = Message::receive(_sockfd, _serverAddr, BUFFER_SIZE);

    if (msg)
    {
        return msg;
    }
    else
    {
        return nullptr;
    }
}

void Client::setTimeout(int timeout)
{
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));    
}

void Client::error(const std::string &message)
{
    std::cerr << message << std::endl;
    exit(1);
}
