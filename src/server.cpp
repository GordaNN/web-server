#include "server.h"

Server::Server(const Config& config) :
    threadPool(config.GetThreadLimit(), config.GetResourcesRoot()),
    port(config.GetPort()),
    threadLimit(config.GetThreadLimit()),
    resourcesRoot(config.GetResourcesRoot())
{
}

bool Server::Start(std::ostream& logOut, std::ostream& errOut)
{
    /* Init useage of the Winsock DLL by a process */
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        errOut << "Can't init Winsock DLL" << std::endl;
        return false;
    }

    /* Create server socket */
    if ((this->server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        errOut << "Can't create socket" << std::endl;
        return false;
    }

    /* Bind server socket */
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->port);

    int addressLength = sizeof(this->address);

    if (bind(this->server, (sockaddr*)&this->address, addressLength) < 0)
    {
        errOut << "Can't bind socket" << std::endl;
        return false;
    }

    if (listen(this->server, 10) != 0)
    {
        errOut << "Listen error" << std::endl;
        return false;
    }

    logOut << "Server started" << std::endl;
    logOut << "port: " << this->port << std::endl;
    logOut << "thread_limit: " << this->threadLimit << std::endl;
    logOut << "resources_root: " << this->resourcesRoot << std::endl;
    logOut << std::endl;

    return true;
}

bool Server::Listen(std::ostream& logOut, std::ostream& errOut)
{
    int addressLength = sizeof(this->address);

    while (true)
    {
        int socket;

        if ((socket = accept(this->server, (struct sockaddr*)&this->address, (int*)&addressLength)) < 0)
        {
            errOut << "Accept error" << std::endl;
            return false;
        }

        int bufferSize = 16384;
        char buffer[16384];
        memset(buffer, 0, bufferSize);
        recv(socket, buffer, bufferSize, 0);
        std::string request(buffer);

        char buf[100];
        time_t now = time(nullptr);
        tm tm = *gmtime(&now);
        strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        logOut << std::string(buf) << std::endl;
        logOut << request.substr(0, request.find('\n')) << std::endl;
        logOut << std::endl;
        this->threadPool.Add(socket, request);
    }

    return true;
}
