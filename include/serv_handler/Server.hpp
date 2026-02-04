#ifndef SERVER_H
#define SERVER_H
#include "parsing/Server_config.hpp"
#include <netinet/in.h>

class Server {
 public:

	Server(); // a supprimer plus tard
	Server(const Server_Config& conf);
	~Server();

	Server&		operator=(const Server& src);

	int					getListenFd() const;
	struct sockaddr_in	getAddr() const;
	Server_Config		getConfig() const;

	void	init(int epoll_fd);
	const Location_config*	sendALocation(const std::string& uri);

 private:
	Server_Config	config;
	int		listen_fd;
	bool	isAlive;
	struct	sockaddr_in addr;
};

#endif
