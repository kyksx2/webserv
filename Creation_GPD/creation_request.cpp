/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creation_request.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:50:59 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/15 16:01:53 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestMethod.hpp"

HTTPRequest	*get_creation(std::string &buffer)
{
	return (new GetRequest(buffer));
}

HTTPRequest	*delete_creation(std::string &buffer)
{
	return (new DeleteRequest(buffer));
}

HTTPRequest	*post_creation(std::string &buffer)
{
	return (new PostRequest(buffer));
}

//Cree la bonne class en fonction du premier mot de la requete | renvoi exception si inconnue
HTTPResponse	RequestCreation(std::string buffer)
{
	HTTPRequest		*request;
	HTTPResponse	response;
	int	i;
	std::string method[] = {"GET", "POST", "DELETE"};
	HTTPRequest *(*ft_method[])(std::vector<std::string>&, std::string &) = {
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
			request = ft_method[i](buffer);
			response = request->generateResponse();
			delete request;
			return (response);
		}
	}
	response.setStatus(400, "Bad Request");
	return (response);
}
