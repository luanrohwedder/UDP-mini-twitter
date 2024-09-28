#include "server.h"
#include <thread>

int main()
{
    Server server(12000);
    server.start();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}