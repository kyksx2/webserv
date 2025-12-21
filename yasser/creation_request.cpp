/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creation_request.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:50:59 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/19 13:40:17 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestMethod.hpp"

HTTPResponse RequestCreation(std::string buffer, const Server& serv)
{
	std::stringstream ss(buffer);
	std::string line;
	std::getline(ss, line);
	std::vector<std::string> firstline = split(line, ' ');
	if (firstline.size() < 3)
		return (HTTPResponse ("HTTP/1.1", 400, "Bad Request"));

	std::string	method = firstline[0];
	std::string version = firstline[2];

	try
	{
		if (method == "GET")
		{
			GetRequest req(buffer, serv);
			return req.generateResponse();
		}
		else if (method == "POST")
		{
			PostRequest req(buffer, serv);
			return req.generateResponse();
		}
		else if (method == "DELETE")
		{
			DeleteRequest req(buffer, serv);
			return req.generateResponse();
		}
		else
		{
			if (version == "HTTP/1.0" || version == "HTTP/1.1")
				return (HTTPResponse (version , 400, "Bad Request"));
		}
		throw std::runtime_error("400 Bad Request");
	}
	catch (const std::exception& e)
	{
		return HTTPResponse("HTTP/1.1", 400, "Bad Request");
	}
}
