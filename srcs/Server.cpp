#include <../include/webserv.hpp>

Server::Server(const ServConf& conf) : config(conf), epoll_fd(-1), listen_fd(-1), isAlive(true) {
    memset(&this->addr, 0, sizeof(addr));
}

Server::~Server() {}

bool    Server::socketInit() {
    this->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listen_fd == -1)  {
        std::cerr << "Error: cannot create socket on port " << config.port << std::endl;
        this->isAlive = false;
        return (false);
    }
    fcntl(this->listen_fd, F_SETFL,  O_NONBLOCK);

    this->addr.sin_addr.s_addr = INADDR_ANY;
    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(config.port);

    int sockaddrlen = sizeof(this->addr);
    if (bind(this->listen_fd, (sockaddr*)&this->addr, sockaddrlen) == -1) {
        std::cerr << "Error: bind failed on port " << config.port << std::endl;
        this->isAlive = false;
        return (false);
    }

    if (listen(listen_fd, SOMAXCONN) == -1) { //? SOMAXCONN --> limite max du noyau
        std::cerr << "Error: socket connection failed on port " << config.port << std::endl;
        this->isAlive = false;
        return (false);
    }
    std::cout << "whait for connection..." << std::endl;
    return (true);
}

bool    Server::epollInit() {
    this->epoll_fd = epoll_create(1);
    if (this->epoll_fd == -1) {
        std::cerr << "Error: epoll failed to create on port " << config.port << std::endl;
        this->isAlive = false;
        return (false);
    }
    struct epoll_event epollEv;
    epollEv.data.fd = this->listen_fd;
    epollEv.events = EPOLLIN;
    epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->listen_fd, &epollEv);
    return (true);
}

void    Server::init() {
    if (!socketInit())
        return;
    if (!epollInit())
        return;
}

void    Server::handleNewClient() {
    int sockaddLen = sizeof(this->addr);
    while (true) {
        int client_fd = accept(this->listen_fd, (sockaddr*)&addr, (socklen_t*)&sockaddLen);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else {
                std::cerr << "Error: socket refused connexion on port " << config.port << std::endl;
                break;
            }
        }
        else {
            fcntl(client_fd, F_SETFL, O_NONBLOCK);
            struct epoll_event client_ev;
            client_ev.data.fd = client_fd;
            client_ev.events = EPOLLIN;
            if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1) {
                std::cerr << "Error: epoll_ctl failed on port " << config.port << std::endl;
                close(client_fd);
                continue;
            }
            Client* new_client = new Client(client_fd);
            this->clients[client_fd] = new_client;
        }
    }
    return;
}

void    Server::sendClientData(int event_fd) {

}

void    Server::readClientData(int event_fd) {
    Client* client = this->clients[event_fd];
    char buffer[BUFFER_SIZE];
    int receiveBits;

    while((receiveBits = recv(event_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        client->appendRequest(buffer, receiveBits);
        if (client->completeRequest()) {
            client->parseRequest();
            client->generateResponse();
            struct epoll_event changeEvent;
            changeEvent.data.fd = event_fd;
            changeEvent.events = EPOLLOUT;
            if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, &changeEvent) == -1) {
                std::cerr << "Error: epoll_ctl failed on port " << config.port << std::endl;
                closeClient();
                return;
            }
            break;
        }
    }
    if (receiveBits <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        else {
            closeClient();
        }
    }
}

void    Server::handleClientIO(int event_fd, uint32_t events) {
    if (events & EPOLLERR | EPOLLHUP | EPOLLRDHUP) {
        close(event_fd);
        return;
    }
    if (events & EPOLLIN)
        readClientData(event_fd);
    if (events & EPOLLOUT)
        sendClientData(event_fd);
}

void    Server::runServ() {
    while(this->isAlive) {
        struct epoll_event ev[EVENTS_MAX];
        int n = epoll_wait(this->epoll_fd, ev, EVENTS_MAX, -1);
        if (n == -1) {
            std::cerr << "Error: epoll_wait failed on port " << config.port << std::endl;
            continue;
        }
        for (int i = 0; i < n; i++) {
            int event_fd = ev[i].data.fd;
            if (event_fd == this->listen_fd)
                handleNewClient();
            else
                handleClientIO(event_fd, ev[i].events);
        }
    }
}
