#include <utility>
#include <WinSock2.h>
#include "handler.h"
#include "thread_pool.h"

ThreadPool::ThreadPool(unsigned int threadLimit, std::string resourcesRoot) :
    isDone(false),
    threadLimit(threadLimit),
    resourcesRoot(std::move(resourcesRoot))
{
    if (this->threadLimit == 0)
    {
        this->threadLimit = 1;
    }

    this->threads.reserve(this->threadLimit);
    for (int i = 0; i < this->threadLimit; ++i)
    {
        this->threads.emplace_back(&ThreadPool::DoWork, this);
    }
}

ThreadPool::~ThreadPool()
{
    this->isDone = true;

    this->queueConditionalVariable.notify_all();
    for (auto &thread : this->threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void ThreadPool::Add(int socket, const std::string& requestString)
{
    std::lock_guard<std::mutex> g(this->queueMutex);
    this->queue.push(std::pair<int, std::string>(socket, requestString));
    this->queueConditionalVariable.notify_one();
}

void ThreadPool::DoWork()
{
    while (!this->isDone)
    {
        std::pair<int, std::string> request;

        {
            std::unique_lock<std::mutex> g(this->queueMutex);
            this->queueConditionalVariable.wait(g, [&]{
                return !this->queue.empty() || this->isDone;
            });

            request = this->queue.front();
            this->queue.pop();
        }

        this->HandleRequest(request.first, request.second);
    }
}

void ThreadPool::HandleRequest(int socket, const std::string& requestString)
{
    Handler handler(this->resourcesRoot);
    std::string message = handler.Handle(socket, requestString);
    send(socket, message.c_str(), message.length(), 0);
    closesocket(socket);
}
