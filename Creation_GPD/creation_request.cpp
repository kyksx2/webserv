/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creation_request.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:50:59 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/12 16:03:10 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestMethod.hpp"

HTTPRequest	*get_creation(std::vector<std::string> &v)
{
	return (new GetRequest(v));
}

HTTPRequest	*delete_creation(std::vector<std::string> &v)
{
	return (new DeleteRequest(v));
}

HTTPRequest	*post_creation(std::vector<std::string> &v)
{
	return (new PostRequest(v));
}

//Cree la bonne class en fonction du premier mot de la requete | renvoi exception si inconnue
HTTPResponse	RequestCreation(std::string buffer)
{
	HTTPRequest		*request;
	HTTPResponse	response;
	std::vector<std::string>	v_request;
	int	i;
	std::string method[] = {"GET", "POST", "DELETE"};
	HTTPRequest *(*ft_method[])(std::vector<std::string>&) = {
		get_creation,
		delete_creation,
		post_creation
	};

	v_request = split(buffer, ' '); // permet de transformer le string en vector en utilisant ' ' comme separateur

	for (i = 0; i < 3; i++)
	{
		if (method[i] == v_request[0])
		{
			request = ft_method[i](v_request);
			response = request->generateResponse();
			delete request;
			return (response);
		}
	}
	response.setStatus(400, "Bad Request");
	return (response);
}
