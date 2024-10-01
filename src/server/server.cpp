#include "server.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <ctime>
#include <iomanip>
#include <sys/time.h>

// Instância da classe para ser acessda globalmente (para sinais)
Server* _serverInstance = nullptr;

// Método para o sinal
void handleStatusBroadcast(int signal)
{
    if (signal == SIGALRM)
    {
        if (_serverInstance != nullptr)
            _serverInstance->sendServerStatus();
    }
}

Server::Server(int port) : _idCount(1), _running(false)
{
    if ((_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        error("Failed to create socket");

    memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(port);

    if (bind(_sockfd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
        error("Failed to bind server address");

    startTime = std::chrono::steady_clock::now();

    _file.open("log.txt");
}

Server::~Server()
{
    _running = false;
    _file.close();
    close(_sockfd);
}

void Server::start()
{
    _running = true;

    _serverInstance = this;

    // Thread para ouvir mensagens
    std::thread listener(&Server::listen, this);
    listener.detach();

    // Timer e Sinal para envio de status
    setupTimer();

    std::cout << "Server ir running..." << std::endl;
}

void Server::listen()
{
    while (_running)
    {
        struct sockaddr_in clientAddr;
        memset(&clientAddr, 0, sizeof(clientAddr));
        Message* msg = Message::receive(_sockfd, clientAddr, BUFFER_SIZE);

        if (msg)
        {
            if ((msg->getType() == Message::OI))
                addClient(clientAddr, msg);

            if ((msg->getType() == Message::TCHAU))
                deleteClient(clientAddr, msg);

            if ((msg->getType() == Message::MSG))
            {
                if (clientExists(msg->getOriginID()))
                {
                    Message* cpyMsg = new Message(*msg);
                    std::thread clientThread(&Server::handleClient, this, cpyMsg);
                    clientThread.detach();
                }
                else
                {
                    Message error(Message::ERRO, 0, msg->getOriginID(), 
                                  msg->getUsername(), "Você não está registrado no sistema!");
                    error.send(_sockfd, clientAddr);
                }
            }

            if ((msg->getType() == Message::LIST))
                handleClientListRequest(clientAddr, msg);

            delete msg;
        }
        else
        {
            error("recvfrom error");
        }
    }
}

void Server::setupTimer()
{
    struct sigaction sigHandler;
    struct itimerval timer;

    sigHandler.sa_handler = handleStatusBroadcast;
    sigemptyset(&sigHandler.sa_mask);
    sigHandler.sa_flags = 0;

    if (sigaction(SIGALRM, &sigHandler, NULL) == -1)
        error("Error: cant set signal handle");
    

    timer.it_value.tv_sec = TIMER;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = TIMER;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, nullptr) == -1)
        error("Error: cant set timer");
}


void Server::handleClient(Message *message)
{
    if (message->getDestinationID() == 0)
        broadcastMessage(message);
    else
        privateMessage(message);

    delete message;
}

void Server::handleClientListRequest(struct sockaddr_in clientAddr, Message *message)
{
    std::string clientList;

    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (const auto &client : _clients)
    {
        if (client.first != message->getOriginID())
            clientList += std::to_string(client.first) + ":" + 
                          client.second.username + "\n";
    }

    Message reply(Message::LIST, 0, message->getOriginID(), _serverID, clientList);
    reply.send(_sockfd, clientAddr);
}

void Server::broadcastMessage(Message *message)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (const auto &client : _clients)
    {
        const sockaddr_in &addr = client.second.address;
        Message msg;
        memcpy(&msg, message, sizeof(Message));
        msg.send(_sockfd, addr);
    }
}

void Server::privateMessage(Message *message)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);

    if (clientExists(message->getDestinationID()))
    {
        const auto client = _clients.find(message->getDestinationID());
        message->send(_sockfd, client->second.address);
    }
    else
    {
        const auto client = _clients.find(message->getOriginID());
        Message error(Message::ERRO, 0, message->getOriginID(), 
                      message->getUsername(), "Usuário não encontrado!");
        error.send(_sockfd, client->second.address);
    }
}

void Server::sendServerStatus()
{
    std::string message = "STATUS: " + _serverID + 
                          " | Clientes: " + std::to_string(_clients.size()) + 
                          " | Tempo: " + getElapsedTime();

    if (message.size() > 140)
        message = message.substr(0, 140);

    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (const auto &client : _clients)
    {
        Message msg(Message::MSG, 0, client.first, _serverID, message);
        const sockaddr_in &addr = client.second.address;
        msg.send(_sockfd, addr);
    }
}

void Server::addClient(struct sockaddr_in clientAddr, Message* msg)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    if (!clientExists(msg->getOriginID()))
    {
        _clients[_idCount] = {clientAddr, msg->getUsername()};

        Message idMessage(Message::OI, 0, _idCount, _serverID, "");
        idMessage.send(_sockfd, clientAddr);

        log(msg, clientAddr, true);

        _idCount++;
    }
}

void Server::deleteClient(struct sockaddr_in clientAddr, Message* msg)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    if (clientExists(msg->getOriginID()))
    {
        _clients.erase(msg->getOriginID());

        log(msg, clientAddr, false);
    }
}

bool Server::clientExists(int clientID)
{
    return _clients.find(clientID) != _clients.end();
}

std::string Server::getElapsedTime()
{
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>
                          (currentTime - startTime).count();

    int hours = elapsedTime / 3600;
    int minutes = (elapsedTime % 3600) / 60;
    int seconds = elapsedTime % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;

    return oss.str();
}

std::string Server::getCurrentTime()
{
    std::time_t now;
    std::time(&now);
    return ctime(&now);
}

void Server::log(Message*  msg, struct sockaddr_in clientAddr, bool isAdd)
{
    if (isAdd)
    {
        std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr)
                  << ":" << ntohs(clientAddr.sin_port) 
                  << " with ID: " << _idCount << std::endl;

        _file << "Connected: " << msg->getUsername() << "#" << _idCount 
              << " -  " << getCurrentTime();
        _file.flush();
    }
    else
    {
        std::cout << "Client disconnected: " << inet_ntoa(clientAddr.sin_addr)
                  << ":" << ntohs(clientAddr.sin_port) 
                  << " with ID: " << msg->getOriginID() << std::endl;

        _file << "Disconnected: " << msg->getUsername() << "#" << msg->getOriginID() 
              << " -  " << getCurrentTime();
        _file.flush();
    }
}

void Server::error(const std::string &message)
{
    std::cerr << message << std::endl;
    exit(1);
}
