#include <iostream>
#include "config.h"
#include "server.h"

int main(int argc, char* argv[])
{
    Config config;
    if (!config.Read(std::cout, std::cerr))
    {
        return -1;
    }

    Server s(config);
    
    if (!s.Start(std::cout, std::cerr))
    {
        return -1;
    }
    
    if (!s.Listen(std::cout, std::cerr))
    {
        return -1;
    }

    return 0;
}
