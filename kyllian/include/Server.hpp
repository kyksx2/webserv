#ifndef SERVER_H
#define SERVER_H
#include "Server_config.hpp"
#include <netinet/in.h>

class Server {
 public:

//   Server(const Server_Config& conf);
Server(); // a supprimer plus tard

  ~Server();
  int getListenFd();
  struct sockaddr_in getAddr();
  // int getPort();
  void init(int epoll_fd);
  Server_Config config;

  private:
  int listen_fd;
  bool isAlive;
  struct sockaddr_in addr;
};


#endif
