/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:45:23 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/15 15:45:30 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::vector<std::string> &v, std::string &buffer)
{
	std::stringstream	ss(buffer);
	std::vector<std::string>::iterator	it = v.begin();
	it++;//skip le 1er mot

	size_t pos = (*it).find('?');// target/query   | verifie la presence de l argument optionel
	if (pos != std::string::npos)
	{		//separe le maillon avant/apres "?"
		this->m_target = (*it).substr(0, pos);
		this->m_query = (*it).substr(pos + 1);
	}
	else
	{
		this->m_target = (*it);
	}
	if (this->m_target == "")
		throw();//Bad request

	if (++it != v.end())//Version
	{
		if ((pos = (*it).find("\r\n")) != std::string::npos)
		{
			this->m_version = (*it).substr(0, pos);
			if (this->m_version != "HTTP/1.0" && this->m_version !="HTTP/1.1")
				throw();//Not implemented
		}
		else
			throw();//Bad request
	}

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
			if (this->m_headers.find(key) != this->m_headers.end()) // verifie existe deja si oui ajoute ", valeur"
				this->m_headers[key] += ", " + value;
			else
				this->m_headers[key] = value;
		}
	}


	if ((pos = buffer.find("\r\n\r\n")) != std::string::npos)
	{
		size_t body_start = pos + 4;

		if (m_headers.count("Content-Length")) // si pas de Content-Length pas de body
		{
			size_t len = std::atoi(m_headers["Content-Length"].c_str());
			this->m_body = buffer.substr(body_start, len);
		}
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
