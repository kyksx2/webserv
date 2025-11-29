/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 14:31:26 by kjolly            #+#    #+#             */
/*   Updated: 2025/11/29 10:44:24 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H
#include <webserv.hpp>

#define EVENTS_MAX 1024
#define BUFFER_SIZE 1024

class Server {
 public:
  Server(const ServConf& conf);
  ~Server();

  void init();
  bool socketInit();
  bool epollInit();
  void runServ();
  void handleNewClient();
  void handleClientIO(int fd, uint32_t event);
  void readClientData(int event_fd);
  void sendClientData(int event_fd);
  void closeClient();

 private:
  ServConf config;
  std::map<int, Client*> clients;
  int listen_fd;
  int epoll_fd;
  bool isAlive;
  struct sockaddr_in addr;
};

#endif