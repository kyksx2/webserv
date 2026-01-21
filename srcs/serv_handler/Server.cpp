#include "serv_handler/Server.hpp"
#include "serv_handler/Webserv.hpp"

Server::Server() {}

Server::Server(const Server_Config& conf) : config(conf), listen_fd(-1), isAlive(true) {
    memset(&this->addr, 0, sizeof(addr));
}

Server& Server::operator=(const Server& src) {
    if (this != &src) {
        this->addr = src.addr;
        this->config = src.config;
        this->isAlive = src.isAlive;
        this->listen_fd = src.listen_fd;
    }
    return (*this);
}

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

const Location_config*  Server::sendALocation() {
    return this->config.findLocation(); //!!! debrouillez vous
}

int Server::getListenFd() const { return this->listen_fd; }

sockaddr_in Server::getAddr() const { return this->addr; }

std::string Server::getHost() const { return this->config.getHost(); }

int Server::getPort() const {
    return this->config.getPort();
}

