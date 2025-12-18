/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creation_request.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:50:59 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/18 14:53:16 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestMethod.hpp"

HTTPRequest	*get_creation(std::string &buffer, const Server& serv)
{
	return (new GetRequest(buffer, serv));
}

HTTPRequest	*delete_creation(std::string &buffer, const Server& serv)
{
	return (new DeleteRequest(buffer, serv));
}

HTTPRequest	*post_creation(std::string &buffer, const Server& serv)
{
	return (new PostRequest(buffer, serv));
}

//Cree la bonne class en fonction du premier mot de la requete | renvoi exception si inconnue
HTTPResponse	RequestCreation(std::string buffer, const Server& serv)
{
	HTTPRequest		*request;
	HTTPResponse	response;
	int	i;
	std::string method[] = {"GET", "POST", "DELETE"};
	HTTPRequest *(*ft_method[])(std::string &, const Server&) = {
		get_creation,
		delete_creation,
		post_creation
	};

	std::stringstream ss(buffer);
	std::string method_request;
	ss >> method_request; // Recupere le premier mot du buffer

	for (i = 0; i < 3; i++)
	{
		if (method[i] == method_request)
		{
			request = ft_method[i](buffer, serv);
			response = request->generateResponse();
			delete request;
			return (response);
		}
	}

	std::stringstream ss(buffer);
	std::string line;
	std::getline(ss, line);
	std::vector<std::string> firstline = split(line, ' ');
	if (firstline.size() < 3)
		throw std::runtime_error("400 Bad Request");

	std::string target = firstline[1];
	std::string version = firstline[2];

	if (version != "HTTP/1.0" && version != "HTTP/1.1")
		version = "HTTP/1.1";
	HTTPResponse response(version , 400, "Bad Request");
	return (response);
}
