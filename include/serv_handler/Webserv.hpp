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
#include "parsing/Global_Config.hpp"
#include "parsing/Server_config.hpp"

#define EVENTS_MAX 1024
#define BUFFER_SIZE 1024
#define RETURN_SIZE 1024

extern volatile sig_atomic_t signal_running;
//? EXTERN: pour dire au compilateur que cette var sera utiliser autre part, donc ne pas la redefinir
//? VOLATILE: empeche le compilateur de faire de l'optimisation sur cette var
//? sig_atomic_t: type de var speciale concu pour communiquer avec des signaux

class WebServ {
 public:
	WebServ(const std::string& conf);
	~WebServ();

	void	run();
	void	epollInit();
	void	handleNewClient(Server* find_server);
	void	readClientData(int event_fd);
	void	sendClientData(int event_fd);
	void	closeClient(int event_fd);
	void	checkTimeout();

	// void printEverythings();

	class initException : public std::exception {
		public:
			const char* what() const throw();
	};
 private:
	std::map<int, Server*>	servers;
	std::map<int, Client*>	clients;
	int						epoll_fd;
};

#endif