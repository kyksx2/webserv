#include "serv_handler/Webserv.hpp"

WebServ::WebServ(const std::string& conf): epoll_fd(-1) {
	epollInit();
	std::vector<Server_Config> serv_conf;
	try
	{
		Global_Config config;
		if (!conf.empty())
			config = Global_Config(conf);

		serv_conf = config.getConfVect();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw initException();
	}
	for (size_t i = 0; i < serv_conf.size(); i++) {
		Server* new_server = new Server(serv_conf[i]);
		try {
			new_server->init(this->epoll_fd);
			std::cout << new_server->getListenFd() << ": Port: "<< new_server->getConfig().getPort() <<   " Host: " << new_server->getConfig().getHost() << std::endl;
			int listen_fd = new_server->getListenFd();
			this->servers[listen_fd] = new_server;
		}
		catch (initException& e) {
			std::cerr << e.what() << std::endl;
			if (new_server)
				delete new_server;
			throw initException();
		}
	}
}

WebServ::~WebServ() {
	std::map<int, Server*>::iterator it;
	for(it = this->servers.begin(); it != this->servers.end(); it++) {
		if (it->first) {
			close(it->first);
			delete it->second;
		}
	}
	this->servers.clear();
	for(std::map<int, Client*>::iterator ite = this->clients.begin(); ite != this->clients.end(); ite++) {
	if (ite->second != NULL) {
		close(ite->first);
		delete ite->second;
	}
	}
	this->clients.clear();
	if (this->epoll_fd)
		close(this->epoll_fd);
}

void    WebServ::epollInit() {
	this->epoll_fd = epoll_create1(0);
	if (this->epoll_fd == -1)
		return; //! renvoyer erreur 501
		// Après socket() ou accept()
	fcntl(this->epoll_fd, F_SETFD, FD_CLOEXEC);
}

void	WebServ::checkTimeout() {
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();) {
		if (time(NULL) - it->second->getStart() > 60) {
			// std::cout << "Timeout: kick out" << std::endl;
			int fd_to_kill = it->first;
			it++;
			closeClient(fd_to_kill);
		}
		else
			it++;
	}
}

const char* WebServ::initException::what() const throw() {
	return ("init server");
}
