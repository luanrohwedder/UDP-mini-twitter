#include "server.h"
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    if (argc != 3) 
    {
        std::cerr << "Uso: " << argv[0] << " <IP> <Porta>" << std::endl;
        return EXIT_FAILURE;
    }

     std::string ip = argv[1];
    int port = std::stoi(argv[2]);

    Server server(ip, port);
    server.start();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}