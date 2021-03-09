#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <utility>
#include "handler.h"

Handler::Handler(std::string resourcesRoot) :
    resourcesRoot(std::move(resourcesRoot))
{
}

std::string Handler::Handle(int socket, std::string requestString)
{
    Request request = this->ParseRequest(std::move(requestString));
    
    std::string status;
    std::vector<Header> headers =
    {
            Header{"Server", "web-server"},
            Header{"Date", GetDate()},
            Header{"Connection", "close"}
    };
    std::string body;

    /* Methods */
    if (request.method == "OPTIONS")
    {
        status = SERVER_CODE_204;
        headers.push_back(Header{ "Allow", "OPTIONS, GET, HEAD, POST, PUT, DELETE" });
        headers.push_back(Header{ "Content-Length", "0" });
    }
    else if (request.method == "GET")
    {
        std::pair<std::string, std::string> pair = ReadResFile(request.path);
        status = pair.first;
        if (status == SERVER_CODE_200)
        {
            body = pair.second;

            headers.push_back(Header{ "Content-Type", GetContentType(request.path) });
            headers.push_back(Header{ "Content-Length", std::to_string(pair.second.length() - 1) });
        }
    }
    else if (request.method == "HEAD")
    {
        std::pair<std::string, int> pair = GetResFileSize(request.path);
        status = pair.first;
        if (status == SERVER_CODE_200)
        {
            headers.push_back(Header{ "Content-Type", GetContentType(request.path) });
            headers.push_back(Header{ "Content-Length", std::to_string(pair.second) });
        }
    }
    else if (request.method == "POST")
    {
        status = CreateResFile(request.path, request.content, std::ios_base::app);
    }
    else if (request.method == "PUT")
    {
        status = CreateResFile(request.path, request.content, 0);
    }
    else if (request.method == "DELETE")
    {
        status = DeleteResFile(request.path);
    }
    else
    {
        status = SERVER_CODE_405;
    }

    std::string message = "HTTP/1.1 " + status + MakeHeadersString(headers) + "\r\n\r\n" + body;
    return message;
}

std::pair<std::string, std::string> Handler::ReadResFile(std::string path)
{
    std::string body;
    std::string status = SERVER_CODE_200;

    bool isDir = false;
    if (path.find('.', 0) == std::string::npos)
    {
        if (path[path.length() - 1] != '/')
        {
            path += "/";
        }
        path += "index.html";
        isDir = true;
    }

    if (CheckPathEscape(path))
    {
        return std::make_pair(SERVER_CODE_403, body);
    }

    std::ifstream file(resourcesRoot + path);
    if (file)
    {
        file.seekg(0, std::ios::end);
        size_t len = file.tellg();
        file.seekg(0);

        body = std::string(len + 1, '\0');
        file.read(&body[0], len);
    }
    else
    {
        if (isDir)
        {
            status = SERVER_CODE_403;
        }
        else
        {
            status = SERVER_CODE_404;
        }
    }

    return std::make_pair(status, body);
}

std::pair<std::string, int> Handler::GetResFileSize(std::string path)
{
    std::string status = SERVER_CODE_200;
    size_t size = 0;

    bool isDir = false;
    if (path.find('.', 0) == std::string::npos)
    {
        if (path[path.length() - 1] != '/')
        {
            path += "/";
        }
        path += "index.html";
        isDir = true;
    }

    if (CheckPathEscape(path))
    {
        return std::make_pair(SERVER_CODE_403, size);
    }

    std::ifstream file(resourcesRoot + path, std::ifstream::ate | std::ifstream::binary);
    if (file)
    {
        size = file.tellg();
    }
    else
    {
        if (isDir)
        {
            status = SERVER_CODE_403;
        }
        else
        {
            status = SERVER_CODE_404;
        }
    }

    return std::make_pair(status, size);
}

std::string Handler::CreateResFile(std::string path, std::string data, int flags)
{
    if (path.find('.', 0) == std::string::npos)
    {
        return SERVER_CODE_400;
    }

    if (CheckPathEscape(path))
    {
        return SERVER_CODE_403;
    }

    std::string status = SERVER_CODE_201;
    {
        std::ifstream file(resourcesRoot + path, std::ofstream::in);
        if (file)
        {
            status = SERVER_CODE_204;
        }
    }

    std::ofstream file(resourcesRoot + path, std::ofstream::out | flags);
    if (!file)
    {
        return SERVER_CODE_500;
    }

    file.write(data.c_str(), data.length());

    return status;
}

std::string Handler::DeleteResFile(std::string path)
{
    if (path.find('.', 0) == std::string::npos)
    {
        return SERVER_CODE_400;
    }

    if (CheckPathEscape(path))
    {
        return SERVER_CODE_403;
    }

    if (std::remove((resourcesRoot + path).c_str()) != 0)
    {
        return SERVER_CODE_410;
    }

    return SERVER_CODE_204;
}

Request Handler::ParseRequest(std::string requestString)
{
    Request request;
    request.contentLength = "0";
    request.contentType = "";

    size_t i = requestString.find(' ');
    request.method = requestString.substr(0, i);

    requestString = requestString.substr(++i, requestString.length());
    i = requestString.find(' ');
    request.path = DecodePath(requestString.substr(0, i));

    i = requestString.find("Content-Length");
    if (i != std::string::npos)
    {
        requestString = requestString.substr(i + 16, requestString.length());
        i = requestString.find("\r\n");
        request.contentLength = DecodePath(requestString.substr(0, i));
    }

    i = requestString.find("Content-Type");
    if (i != std::string::npos)
    {
        requestString = requestString.substr(i + 14, requestString.length());
        i = requestString.find("\r\n");
        request.contentType = DecodePath(requestString.substr(0, i));
    }

    if (request.contentLength != "0")
    {
        i = requestString.find("\r\n\r\n");
        requestString = requestString.substr(i + 4, requestString.length());
        request.content = DecodePath(requestString.substr(0, std::stoi(request.contentLength)));
    }

    /* Delete query strings */
    i = request.path.find('?', 0);
    if (i != std::string::npos)
    {
        request.path = request.path.substr(0, i);
    }

    return request;
}

std::string Handler::DecodePath(std::string path)
{
    std::string result;

    for (int i = 0; i < path.length(); i++)
    {
        if (path[i] == '%')
        {
            int ch;
            sscanf(path.substr(i + 1, 2).c_str(), "%x", &ch);
            result += static_cast<char>(ch);
            i += 2;
        }
        else if (path[i] == '+')
        {
            result += ' ';
        }
        else
        {
            result += path[i];
        }
    }

    return result;
}

std::string Handler::GetContentType(std::string path)
{
    if (path.length() < 3)
    {
        return "text/html";
    }
    else if (path.substr(path.length() - 3, 3) == ".js")
    {
        return "text/javascript";
    }

    if (path.length() < 4)
    {
        return "text/html";
    }
    else if (path.substr(path.length() - 4, 4) == ".css")
    {
        return "text/css";
    }
    else if (path.substr(path.length() - 4, 4) == ".jpg")
    {
        return "image/jpeg";
    }
    else if (path.substr(path.length() - 4, 4) == ".png")
    {
        return "image/png";
    }
    else if (path.substr(path.length() - 4, 4) == ".gif")
    {
        return "image/gif";
    }
    else if (path.substr(path.length() - 4, 4) == ".swf")
    {
        return "application/x-shockwave-flash";
    }
    
    if (path.length() < 5)
    {
        return "text/html";
    }
    else if (path.substr(path.length() - 5, 5) == ".html")
    {
        return "text/html";
    }
    else if (path.substr(path.length() - 5, 5) == ".jpeg")
    {
        return "image/jpeg";
    }

    return "text/html";
}

std::string Handler::MakeHeadersString(const std::vector<Header>& headers)
{
    std::string result;
    for (auto& header : headers)
    {
        result.append("\r\n" + header.name + ": " + header.value);
    }
    return result;
}

bool Handler::CheckPathEscape(std::string path)
{
    path = path.substr(1, path.length());
    int dirLevel = 0;

    int pos;
    while ((pos = path.find('/', 0)) != std::string::npos)
    {
        std::string dir = path.substr(0, pos);

        if (dir == "..")
        {
            --dirLevel;
        }
        else
        {
            ++dirLevel;
        }

        path = path.substr(pos + 1, path.length());
    }

    return dirLevel < 0;
}

std::string Handler::GetDate()
{
    char buf[100];
    time_t now = time(nullptr);
    tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M^%S %Z", &tm);
    return std::string(buf);
}
