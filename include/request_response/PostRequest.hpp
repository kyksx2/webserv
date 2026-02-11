/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:39:08 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/11 15:03:22 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTRequest_HPP
# define  POSTRequest_HPP

#include "HTTPRequest.hpp"
class Client;

class PostRequest : public HTTPRequest
{
	public:
		PostRequest(std::string &buffer, const Server& serv);
		~PostRequest();
		HTTPResponse	generateResponse();
		char			**generateEnvp();
		void			startCgi(int epoll_fd, std::map<int, Client*>& client_map, Client* client);

};

void	split_path(std::string path, std::string &dir, std::string &file);
#endif