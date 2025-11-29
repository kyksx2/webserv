/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 14:45:21 by kjolly            #+#    #+#             */
/*   Updated: 2025/11/29 12:18:49 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
#define CLIENT_H
#include <webserv.hpp>

class Client {
 public:
   Client(int fd);
   ~Client();
   void appendRequest(const char* request, int size);
   bool completeRequest();
   void parseRequest();
   void generateResponse();
 private:
   int client_fd;
   std::string requestBuffer;
   httpRequest request;
   httpResponse response;
};

#endif