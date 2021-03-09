#include <iostream>
#include <ctime>
#include <WinSock2.h>
#include "config.h"
#include "thread_pool.h"
#include "handler.h"

int main(int argc, char* argv[])
{
    /* Read command line arguments */
    unsigned int port = 3000; /* Default port value */
    for (int i = 1; i < argc; ++i)
    {
        if (i + 1 != argc)
        {
            if (std::string(argv[i]) == "-p")
            {
                ++i;
                try
                {
                    port = std::stoi(std::string(argv[i]));
                }
                catch (...)
                {
                    std::cerr << "Wrong -p argument" << std::endl;
                }
            }
        }
    }

    /* Read config */
    Config config;
    if (!config.Read(std::cout, std::cerr))
    {
        return -1;
    }

    /* Init useage of the Winsock DLL by a process */
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        std::cerr << "Can't init Winsock DLL" << std::endl;
        return -1;
    }

    /* Create server socket */
    int server;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Can't create socket" << std::endl;
        return -1;
    }

    /* Bind server socket */
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int addressLength = sizeof(address);

    if (bind(server, (sockaddr*)&address, addressLength) < 0)
    {
        std::cerr << "Can't bind socket" << std::endl;
        return -1;
    }

    if (listen(server, 10) != 0)
    {
        std::cerr << "Listen error" << std::endl;
        return -1;
    }

    ThreadPool threadPool(config.GetThreadLimit(), config.GetResourcesRoot());

    std::cout << "Server started" << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "thread_limit: " << config.GetThreadLimit() << std::endl;
    std::cout << "resources_root: " << config.GetResourcesRoot() << std::endl;
    std::cout << std::endl;

    /* Listening */
    while (true)
    {
        int socket;

        if ((socket = accept(server, (struct sockaddr*)&address, (int*)&addressLength)) < 0)
        {
            std::cerr << "Accept error" << std::endl;
            return -1;
        }

        int bufferSize = 16384;
        char buffer[16384];
        recv(socket, buffer, bufferSize, 0);
        std::string request(buffer);

        char buf[100];
        time_t now = time(nullptr);
        tm tm = *gmtime(&now);
        strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        std::cout << std::string(buf) << std::endl;
        std::cout << request.substr(0, request.find('\n')) << std::endl;
        std::cout << std::endl;
        threadPool.Add(socket, request);
    }

    return 0;
}
