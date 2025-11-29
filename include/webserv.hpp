#ifndef WEBSERV_H
#define WEBSERV_H

#include <Server.hpp>
#include <Client.hpp>

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>

class Server;

class WebServ {
 public:
    WebServ(std::string &conf);
    ~WebServ();
    void run();
 private:
    std::vector<Server*> servers;
};

#endif