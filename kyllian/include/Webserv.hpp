#ifndef WEBSERV_H
#define WEBSERV_H

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <cerrno>

#include "Server.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "Server_config.hpp"

#define EVENTS_MAX 1024
#define BUFFER_SIZE 1024
#define RETURN_SIZE 1024

class WebServ {
 public:
    WebServ(const std::string& conf);
    ~WebServ();

    void run();
    void epollInit();
    void handleNewClient(Server* find_server);
    void readClientData(int event_fd);
    void sendClientData(int event_fd);
    void closeClient(int event_fd);

   // void printEverythings();

    class initException : public std::exception {
      public:
         const char* what() const throw();
    };
 private:
    std::map<int, Server*> servers;
    std::map<int, Client*> clients;
    int epoll_fd;
};

#endif