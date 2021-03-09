# Web Server
Multithreaded HTTP web server written in C++ based on thread pool architecture for Windows

## Supported HTTP Methods
* OPTIONS
* GET
* HEAD
* POST
* PUT
* DELETE

## Usage
`web-server -p {PORT}`
default port - 3000

## Configuration
File `server.config`
* thread_limit - maximum thred count
* resources_root - server files directory

### used libraries
* ws2_32 

#### developers
Nikolay Numenko
