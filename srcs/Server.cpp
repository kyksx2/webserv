#include "../include/Server.hpp"
#include "../include/Webserv.hpp"

Server::Server(const Server_Config& conf) : config(conf), listen_fd(-1), isAlive(true) {
    memset(&this->addr, 0, sizeof(addr));
}

Server::Server() {}


Server::~Server() {}

void Server::init(int epoll_fd) {
    this->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listen_fd == -1) {
        std::cerr << "Error: cannot create socket on port " << this->config.getPort() << std::endl;
        this->isAlive = false;
        return;
    }
    fcntl(this->listen_fd, F_SETFL, O_NONBLOCK);
    this->addr.sin_addr.s_addr = inet_addr(this->config.getHost().c_str());
    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(this->config.getPort());

    int sockaddrlen = sizeof(this->addr);
    int bd = bind(this->listen_fd, (sockaddr*)&this->addr, sockaddrlen);
    if (bd == -1) {
        if (errno == EADDRINUSE)
            std::cerr << "Error: port " << this->config.getPort() << " already in use" << std::endl;
        else
            std::cerr << "Error: bind on port " << this->config.getPort() << std::endl;
        close(this->listen_fd);
        this->isAlive = false;
        return;
    }
    if (listen(this->listen_fd, SOMAXCONN) == -1) { //? SOMAXCONN --> limite max du noyau
        std::cerr << "Error: socket connection failed on port " << this->config.getPort() << std::endl;
        close(this->listen_fd);
        this->isAlive = false;
        return;
    }
    struct epoll_event epev;
    epev.data.fd = this->listen_fd;
    epev.events = EPOLLIN | EPOLLHUP | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->listen_fd, &epev) == -1) {
        std::cerr << "Error: epoll_ctl failed on port " << this->config.getPort() << std::endl;
        close(this->listen_fd);
        this->isAlive = false;
        return;
    }
}

int Server::getListenFd() { return this->listen_fd; }

sockaddr_in Server::getAddr() { return this->addr; }


// int Server::getPort() {
//     return this->config.getPort();
// }
