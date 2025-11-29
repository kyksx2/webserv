#include <../include/webserv.hpp>

Client::Client(int fd) : client_fd(fd), requestBuffer(""), response(), request() {}

Client::~Client() {}

void    Client::appendRequest(const char* request, int size) {
    if (!request || size <= 0)
        return;
    this->requestBuffer.append(request, size);
}

bool    Client::completeRequest() {

}

void    Client::parseRequest() {

}

void    Client::generateResponse() {

}