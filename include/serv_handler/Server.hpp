#ifndef SERVER_H
#define SERVER_H
#include "parsing/Server_config.hpp"
#include <netinet/in.h>

class Server {
 public:

	Server(); // a supprimer plus tard
	Server(const Server_Config& conf);
	Server& operator=(const Server& src);

	~Server();
	int getListenFd() const;
	struct sockaddr_in getAddr() const;
	std::string getHost() const;
	int getPort() const;
	void init(int epoll_fd);
	
	private:
	Server_Config config;
	int listen_fd;
	bool isAlive;
	struct sockaddr_in addr;
};


#endif
