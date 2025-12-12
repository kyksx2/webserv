/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:45:23 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/12 16:03:35 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::vector<std::string> &v)
{
	std::vector<std::string>::iterator	it = v.begin();
	it++;//skip le 1er mot

	size_t pos = (*it).find('?');// target/query   | verifie la presence de l argument optionel
	if (pos != std::string::npos)
	{		//separe le maillon avant/apres "?"
		target = (*it).substr(0, pos);
		query = (*it).substr(pos + 1);
	}
	else
	{
		target = (*it);
	}
	if (++it != v.end())//Version
		version = (*it);
	for (it; it != v.end(); ++it) // headers
	{
		std::string	key = (*it);
		if (key == "\r\n")
			break;
		it++;
		if (it != v.end())
		{
			std::string	value = (*it);
			while ((value.find("\r\n") == std::string::npos) && it != v.end())
			{
				value += " ";
				value += (*it);
				it++;
			}
			if (headers.find(key) != headers.end()) // verifie existe deja si oui ajoute ", valeur"
				headers[key] += ", " + value;
			else
				headers[key] = value;
		}
	}
	for (it; it != v.end(); ++it) // body
	{
		body += *it;
	}
}

HTTPRequest::~HTTPRequest()
{
}

std::string HTTPRequest::GetTarget() const
{
	return (this->m_target);
}

std::string HTTPRequest::GetQuery() const
{
	return (this->m_query);
}

std::string HTTPRequest::GetVersion() const
{
	return (this->m_version);
}

std::string HTTPRequest::GetBody() const
{
	return (this->m_body);
}

std::map<std::string, std::string> HTTPRequest::GetHeaders() const
{
	return (this->m_headers);
}

HTTPRequest::HTTPRequestExcpetion::HTTPRequestExcpetion() throw()
{
}

const char *HTTPRequest::HTTPRequestExcpetion::what() const throw()
{
	return ("Error: Invalid request");
}

HTTPRequest::HTTPRequestExcpetion::~HTTPRequestExcpetion() throw()
{
}
