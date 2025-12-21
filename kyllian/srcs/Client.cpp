#include "../include/Client.hpp"

Client::Client(int fd, Server* find_server) : client_fd(fd), data_sent(0), dad_serv(find_server), requestBuffer(""), responseBuffer("") {}

Client::Client(const Client& src) {
    *this = src;
}

Client&  Client::operator=(const Client& src) {
    if (this != &src) {
        this->client_fd = src.client_fd;
        this->data_sent = src.data_sent;
        this->dad_serv = src.dad_serv;
        // this->requestBuffer = src.requestBuffer;
        // this->responseBuffer = src.responseBuffer;
        //! pour les 2 classe request et responses verifier qu'il y a bien un constructeur de copie
    }
    return (*this);
}

size_t  Client::getDataSent() { return this->data_sent; }

void    Client::setDataSent(int n) { this->data_sent = n; }

Client::~Client() {}

void    Client::appendRequest(const char* request, int size) {
    if (!request || size <= 0)
        return;
    this->requestBuffer.append(request, size);
}

void    Client::clearState() {
    this->data_sent = 0;
    this->responseBuffer.clear();
    this->requestBuffer.clear();
}

std::string& Client::getResponseBuffer() { return this->responseBuffer; }

void    Client::setResponseBuffer(std::string& response) {
    this->responseBuffer = response;
}

// bool    Client::completeRequest() {

// }

// void    Client::parseRequest() {

// }

// void    Client::generateResponse() {
    
// }