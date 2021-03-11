#ifndef WEB_SERVER_SERVER_H
#define WEB_SERVER_SERVER_H

#include <iostream>
#include <string>
#include <ctime>
#include <WinSock2.h>
#include "config.h"
#include "thread_pool.h"

class Server
{
public:
	Server(const Config& config);

	bool Start(std::ostream& logOut, std::ostream& errOut);
	bool Listen(std::ostream& logOut, std::ostream& errOut);

private:
	ThreadPool threadPool;
	sockaddr_in address;
	int server;
	unsigned int port;
	unsigned int threadLimit;
	std::string resourcesRoot;
};

#endif // WEB_SERVER_SERVER_H
