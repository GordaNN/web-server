#ifndef WEB_SERVER_HANDLER_H
#define WEB_SERVER_HANDLER_H

#include <string>
#include <vector>
#include <map>

#define SERVER_CODE_200 "200 OK"
#define SERVER_CODE_201 "201 Created"
#define SERVER_CODE_204 "204 No Content"
#define SERVER_CODE_400 "400 Bad Request"
#define SERVER_CODE_403 "403 Forbidden"
#define SERVER_CODE_404 "404 Not Found"
#define SERVER_CODE_405 "405 Method Not Allowed"
#define SERVER_CODE_410 "410 Gone"
#define SERVER_CODE_500 "500 Internal Server Error"

struct Request
{
    std::string method;
    std::string path;
    std::string contentLength;
    std::string contentType;
    std::string content;
};

struct Header
{
    std::string name;
    std::string value;
};

class Handler
{
public:
    explicit Handler(std::string resourcesRoot);
    std::string Handle(int socket, std::string requestString);

private:
    std::string resourcesRoot;

    std::pair<std::string, std::string> ReadResFile(std::string path);
    std::pair<std::string, int> GetResFileSize(std::string path);
    std::string CreateResFile(std::string path, std::string data, int flags);
    std::string DeleteResFile(std::string path);

    static Request ParseRequest(std::string requestString);
    static std::string DecodePath(std::string path);
    static std::string GetContentType(std::string path);
    static std::string MakeHeadersString(const std::vector<Header>& headers);
    static bool CheckPathEscape(std::string path);
    static std::string GetDate();    
};

#endif // WEB_SERVER_HANDLER_H
