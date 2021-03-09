#ifndef WEB_SERVER_THREAD_POOL_H
#define WEB_SERVER_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
public:
    ThreadPool(unsigned int threadLimit, std::string resourcesRoot);
    ~ThreadPool();

    void Add(int socket, const std::string& requestString);

private:
    bool isDone;
    unsigned int threadLimit;
    std::string resourcesRoot;
    std::vector<std::thread> threads;
    std::queue<std::pair<int, std::string>> queue;
    std::mutex queueMutex;
    std::condition_variable_any queueConditionalVariable;

    void DoWork();
    void HandleRequest(int socket, const std::string& requestString);
};

#endif // WEB_SERVER_THREAD_POOL_H
