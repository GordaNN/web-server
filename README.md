# Web Server
Multithreaded HTTP web server written in C++ based on thread pool architecture for Windows

## Supported HTTP Methods
* OPTIONS
* GET
* HEAD
* POST
* PUT
* DELETE

## Configuration
File `server.config`
* port - port of server (default 3000)
* thread_limit - maximum thred count (default 1)
* resources_root - server files directory (default "./")

### used libraries
* ws2_32 

#### developers
Nikolay Numenko
