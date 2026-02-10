#include "serv_handler/Webserv.hpp"

void    WebServ::run() {
	while(signal_running) {
		struct epoll_event ev[EVENTS_MAX];
		int n_event = epoll_wait(this->epoll_fd, ev, EVENTS_MAX, 1000);
		if (n_event == -1) {
			if (errno == EINTR)
				continue;
			else {
				std::cerr << "Error: fatal on epoll_wait." << std::endl;
				break;
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
					if (events & (EPOLLIN | EPOLLHUP)) { //? verifie si le bit represente EPOLLIN
						current_client->restartTimer();
						readClientData(event_fd);
						if (this->clients.find(event_fd) == this->clients.end())
						continue;
					}
					if (events & EPOLLOUT) { //? idem
						current_client->restartTimer();
						sendClientData(event_fd);
					}
					if (events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
						closeClient(event_fd);
						continue;
					}
				}
			}
		}
		checkTimeout();
	}
}

void    WebServ::readClientData(int event_fd) {
	Client* client = this->clients[event_fd];
	if (client->getActiveCgi() && event_fd == client->getCgiFd()) {
		char	buffer[4096];
		ssize_t	n = read(event_fd, buffer, sizeof(buffer)); //? event_fd == pipe_from_cgi[0] -> le script
		if (n > 0) {
			client->appendRequestCgi(buffer, n);
		}
		else if (n == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return;
			}
			perror("read cgi pipe");
		}
		if (n == 0 || (n == -1 && errno != EAGAIN)) {
			if(epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, event_fd, NULL) == -1) {
				perror("error epoll_ctl for cgi");
			}
			this->clients.erase(event_fd);
			close(event_fd);
			int status = 0;
			waitpid(client->getCgiPid(), NULL, 0);
			if (WIFEXITED(status)) {
				int exit_code = WEXITSTATUS(status);
				if (exit_code != 0) {
					std::cout << "error on CGI" << std::endl;
					//? generer erreur 500
				}
			}
			client->completeCgi();
			struct epoll_event change_ev_cgi;
			change_ev_cgi.data.fd = client->getClientFd();
			change_ev_cgi.events = EPOLLOUT;
			if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, client->getClientFd(), &change_ev_cgi) == -1) {
				closeClient(client->getClientFd());
			}
		}
		return;
	}
	// Si un CGI est en cours d'exécution pour ce client, on ignore les inputs sur le socket client
	// pour éviter de lancer une nouvelle requête ou de corrompre l'état
	if (client->getActiveCgi()) {
		return;
	}
	char buffer[BUFFER_SIZE];
	int receive_bits;
	while((receive_bits = recv(event_fd, buffer, BUFFER_SIZE, 0)) > 0) {
		client->appendRequest(buffer, receive_bits);
		if (client->completeRequest()) { //! il faut remplir le buffer, le parser et generer la reponse
			client->generateBufferResponse(this->epoll_fd, this->clients, client); //? generer la reponse
			if (client->getActiveCgi())
				break;
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

void    WebServ::closeClient(int event_fd) {
	std::map<int, Client*>::iterator it = this->clients.find(event_fd);
	if (it == this->clients.end()) {
		std::cerr << "Error: Client " << event_fd
		<< " not found for close connexion." << std::endl;
		return;
	}
	Client* close_client = it->second;
	int client_fd = close_client->getClientFd();
	bool is_cgi = close_client->getActiveCgi();
	int cgi_fd = -1;
	pid_t cgi_pid = -1;
	if (is_cgi) {
		cgi_fd = close_client->getCgiFd();
		cgi_pid = close_client->getCgiPid();
	}
	if (is_cgi && this->clients.count(cgi_fd)) {
		this->clients.erase(cgi_fd);
	}
	if (this->clients.count(client_fd)) {
		this->clients.erase(client_fd);
	}
	if (is_cgi) {
		epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, cgi_fd, NULL);
		close(cgi_fd);
		kill(cgi_pid, SIGKILL);
		waitpid(cgi_pid, NULL, 0);
	}
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1) {
		std::cerr << "Error: epoll_ctl failed for delete client " <<
		client_fd << std::endl;
	}
	if (close(client_fd) == -1) {
		std::cerr << "Error: failed to close client " << client_fd
		<< std::endl;
	}
	delete close_client;
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
			<< find_server->getConfig().getPort()
			<< std::endl;
			return;
		}
	}
	else {
		fcntl(client_fd, F_SETFL, O_NONBLOCK); //? passage en non bloquant
		// Après socket() ou accept()
		fcntl(client_fd, F_SETFD, FD_CLOEXEC); //?
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
