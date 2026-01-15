#include "../include/Webserv.hpp"

// // WebServ::WebServ(std::string& conf): epoll_fd(-1) {
// //     epollInit();
// //     std::vector<ServConf> server_config;
// //     confParser config(conf);
// //     try
// //     {
// //         server_config = config.parse();
// //     }
// //     catch(const std::exception& e)
// //     {
// //         std::cerr << "Error: " << e.what() << '\n';
// //         throw initException();
// //     }

// //     for (size_t i = 0; server_config.size(); i++) {
// //         Server* new_server = new Server(server_config[i]);
// //         try {
// //             new_server->init(this->epoll_fd);
// //             int listen_fd = new_server->getListenFd();
// //             this->servers[listen_fd] = new_server;
// //         }
// //         catch (initException& e) {
// //             std::cerr << "Error: " << e.what() << std::endl;
// //             if (new_server)
// //                 delete new_server;
// //             throw initException();
// //         }
// //     }
// // }

// // WebServ::WebServ(const std::string& conf): epoll_fd(-1) {
// //     epollInit();
// //     std::vector<Server_Config> serv_conf;
// //     try
// //     {
// //         Config config(conf);
// //         serv_conf = config.getConfVect();
// //     }
// //     catch(const std::exception& e)
// //     {
// //         std::cerr << e.what() << '\n';
// //         throw initException();
// //     }
// //     for (size_t i = 0; i < serv_conf.size(); i++) {
// //         Server* new_server = new Server(serv_conf[i]);
// //         try {
// //             new_server->init(this->epoll_fd);
// //             std::cout << new_server->getListenFd() << ": Port: "<< new_server->config.getPort() <<   " Host: " << new_server->config.getHost() << std::endl;
// //             int listen_fd = new_server->getListenFd();
// //             this->servers[listen_fd] = new_server;
// //         }
// //         catch (initException& e) {
// //             std::cerr << e.what() << std::endl;
// //             if (new_server)
// //                 delete new_server;
// //             throw initException();
// //         }
// //     }
// // }

WebServ::WebServ(const std::string& conf): epoll_fd(-1) {
	epollInit();
	std::vector<Server_Config> serv_conf;
	try
	{
		Global_Config config(conf);
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
			std::cout << new_server->getListenFd() << ": Port: "<< new_server->config.getPort() <<   " Host: " << new_server->config.getHost() << std::endl;
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

const char* WebServ::initException::what() const throw() {
	return ("init server");
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
}

void    WebServ::epollInit() {
	this->epoll_fd = epoll_create1(0);
	if (this->epoll_fd == -1)
		return; //! gerer epoll create error
}

void    WebServ::readClientData(int event_fd) { //!!!!!! verifier avec yasser
	Client* client = this->clients[event_fd];
	char buffer[BUFFER_SIZE];
	int receive_bits;
	//? ---------------------------- test --------------------------------
// 	receive_bits = recv(event_fd, buffer, BUFFER_SIZE - 1, 0);
	
// 	if (receive_bits > 0) {
// 		buffer[receive_bits] = '\0'; // Pour l'afficher comme une string
// 		std::cout << "Reçu du client " << event_fd << ": " << buffer << std::endl;
// 		std::string raw_response = "Salut ! J'ai bien reçu ton message.\n";
		
// 		client->setResponseBuffer(raw_response); 
	
// 		struct epoll_event change_ev;
// 		change_ev.data.fd = event_fd;
// 		change_ev.events = EPOLLOUT;
// 		epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, &change_ev);
// 	}
// 	else if (receive_bits == 0) {
// 		closeClient(event_fd);
// 	}
// 	else {
// 		if (errno == EAGAIN || errno == EWOULDBLOCK)
// 			return;
// 	}
// }
	//? ------------------------------ fin test ----------------------------
    while((receive_bits = recv(event_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        client->appendRequest(buffer, receive_bits);
        if (client->completeRequest()) { //! il faut remplir le buffer, le parser et generer la reponse
            // client->(); //? generer la reponse
            std::cout << client->getResponseBuffer() << std::endl;
            struct epoll_event change_ev;
            change_ev.data.fd = event_fd;
            change_ev.events = EPOLLOUT;
            if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, &change_ev) == -1) { //? le client est pret a recevoir la reponse
                std::cerr << "Error: epoll_ctl failed on client "
                << client->getClientFd() << std::endl;
                closeClient(event_fd);
                return;
            }
            break;
        }
    }
    if (receive_bits <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        else {
            closeClient(event_fd);
        }
    }
}

void    WebServ::sendClientData(int event_fd) {
    Client* client = this->clients[event_fd];
    const std::string& message_send = client->getResponseBuffer();
    size_t sent_bytes = client->getDataSent(); //? on recupere ce qu'on a deja envoyer
    size_t total_size = message_send.size();
    const char* messagebuffer = message_send.c_str() + sent_bytes; //? sent_bytes nous permet de savoir ou mettre le pointeur de debut
    size_t remaining_bites = total_size - sent_bytes; //? ce qu'il reste a envoyer

    ssize_t sent = 0; 
    sent = send(event_fd, messagebuffer, remaining_bites, 0);
    if (sent < 0) {
        std::cerr << "Error: sending failed on client " << client->getClientFd() << std::endl;
        return;
    }
    client->setDataSent(sent);
    if (client->getDataSent() >= total_size) {
        if (client->isKeepAlive()) {
            struct epoll_event change_ev;
            change_ev.data.fd = event_fd;
            change_ev.events = EPOLLIN;
            if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, &change_ev) == -1) { //? modif en EPOLLIN
                std::cerr << "Error: epoll_ctl failed on client "
                << client->getClientFd() << std::endl;
                closeClient(event_fd);
                return;
            }
            client->clearState(); //? remet les conteurs a 0
        }
        else
            closeClient(event_fd);
    }
}

//? ------------------------------------ test ----------------------------------
// void    WebServ::sendClientData(int event_fd) {
// 	Client* client = this->clients[event_fd];
	
// 	const std::string& message_send = client->getResponseBuffer();
	
// 	size_t sent_bytes = client->getDataSent();
// 	size_t total_size = message_send.size();
// 	const char* messagebuffer = message_send.c_str() + sent_bytes;
// 	size_t remaining_bytes = total_size - sent_bytes;

// 	ssize_t sent = send(event_fd, messagebuffer, remaining_bytes, 0);

// 	if (sent < 0) {
// 		if (errno == EAGAIN || errno == EWOULDBLOCK)
// 			return;
		
// 		std::cerr << "Error: sending failed on client " << event_fd << std::endl;
// 		closeClient(event_fd);
// 		return;
// 	}

// 	client->setDataSent(sent_bytes + sent);
// 	if (client->getDataSent() >= total_size) {
// 		struct epoll_event change_ev;
// 		change_ev.data.fd = event_fd;
// 		change_ev.events = EPOLLIN; // <--- On veut lire la prochaine phrase
		
// 		if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, &change_ev) == -1) {
// 			std::cerr << "Error: epoll_ctl failed" << std::endl;
// 			closeClient(event_fd);
// 			return;
// 		}
// 		client->clearState(); 
		
// 		std::cout << "Message envoyé, retour en écoute (EPOLLIN)" << std::endl;
// 	}
// }
//? ----------------------------------- fin test -------------------------------

void    WebServ::closeClient(int event_fd) {
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, event_fd, NULL) == -1) {
		std::cerr << "Error: epoll_ctl failed for delete client " <<
		event_fd << std::endl;
	}
	if (close(event_fd) == -1) {
		std::cerr << "Error: failed to close client " << event_fd
		<< std::endl;
	}
	std::map<int, Client*>::iterator it = this->clients.find(event_fd);
	if (it != this->clients.end()) {
		if (it->second)
			delete it->second; //? second correspond au client, first a la cle
		this->clients.erase(it); //? sort de la map
		std::cout << "Client " << event_fd
		<< " closed and cleaned." << std::endl;
	}
	else {
		std::cerr << "Error: Client " << event_fd
		<< " not found for close connexion." << std::endl;
	}
}

void    WebServ::run() {
	while(signal_running) {
		struct epoll_event ev[EVENTS_MAX];
		int n_event = epoll_wait(this->epoll_fd, ev, EVENTS_MAX, 1000);
		if (n_event == -1) {
			if (errno == EINTR)
				continue;
			else {
				std::cerr << "Error: fatal on epoll_wait." << std::endl;
				exit(1);
			}
		}
		for (int i = 0; i < n_event; i++) {
			int event_fd = ev[i].data.fd;
			u_int32_t events = ev[i].events;
//? find return un iterateur qui correspond a la valeur chercher dans la map
//? end signifie que l'iteratteur est hors champ, il n'a pas ete trouver
			std::map<int, Server*>::iterator it_server = this->servers.find(event_fd);
			if (it_server != this->servers.end()) {
					//? on a trouver un server existant donc accepter une nvl connexion
					Server* find_server = it_server->second;
					handleNewClient(find_server);
			}
			else {
				//! aucun server n'a ete trouver donc ca doit etre un client existant
				//? a partir d'ici un client existant a ete trouver
				std::map<int, Client*>::iterator it_client = this->clients.find(event_fd);
				if (it_client != this->clients.end()) {
					Client* current_client = it_client->second;
					if (events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
						closeClient(event_fd);
						continue;
					}
					if (events & EPOLLIN) { //? verifie si le bit represente EPOLLIN
						current_client->restartTimer();
						readClientData(event_fd);
					}
					if (events & EPOLLOUT) { //? idem
						current_client->restartTimer(); 
						sendClientData(event_fd); 
					}
				}
			}
		}
		checkTimeout();
	}
}

void	WebServ::checkTimeout() {
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();) {
		if (time(NULL) - it->second->getStart() > 60) {
			std::cout << "Timeout: kick out" << std::endl;
			int fd_to_kill = it->first;
			it++;
			closeClient(fd_to_kill);
		}
		else
			it++;
	}
}

void    WebServ::handleNewClient(Server* find_server) {
	struct sockaddr_in client_addr;
	int sockLen = sizeof(client_addr);

	int client_fd = accept(find_server->getListenFd(), (sockaddr*)&client_addr, (socklen_t*)&sockLen);
	if (client_fd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return;
		}
		else {
			std::cerr << "Error: socket refused connexion on port "
			//<< find_server->getPort() 
			<< std::endl; //! mettre le port ici
			return;
		}
	}
	else {
		fcntl(client_fd, F_SETFL, O_NONBLOCK); //? passage en non bloquant
		struct epoll_event client_ev;
		client_ev.data.fd = client_fd;
		client_ev.events = EPOLLIN; //? pret a envoyer quelque chose
		if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1) {
			std::cerr << "Error: epoll_ctl failed for client "
			<< client_fd << std::endl; //! mettre le port ici
			close(client_fd);
			return;
		}
		Client* new_client = new Client(client_fd, find_server);
		this->clients[client_fd] = new_client; //? ajout dans la map
	}
	return;
}

// //           UTILS PRINT

// // void WebServ::printEverythings() {
// //     std::map<int, Server*>::iterator it;

//     for (it = this->servers.begin(); it != this->servers.end(); ++it) {
		
//         // it->first  : CLÉ (le int, le Listen FD)
//         // it->second : VALEUR (le Server*)
//         std::cout << "Server sur le FD : " << it->first << std::endl;

// //         if (it->second) // Petite sécurité pour vérifier que le pointeur n'est pas null
// //             it->second->config.print();
// //     }
// // }
