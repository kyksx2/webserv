#ifndef SERVER_H
#define SERVER_H
#include "webserv.hpp"
#include "Servconf.hpp"
#include <netinet/in.h>

class Server {
 public:
  Server(const ServConf& conf);
  ~Server();
  int getListenFd();
  struct sockaddr_in getAddr();
  // int getPort();
  void init(int epoll_fd);
  ServConf config;

 private:
  int listen_fd;
  bool isAlive;
  struct sockaddr_in addr;
};

#endif