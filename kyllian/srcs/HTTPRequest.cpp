/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:45:23 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/23 15:06:38 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HTTPRequest.hpp"

std::vector<std::string>	split(const std::string &chaine, char delimiteur)
{
	std::vector<std::string>	v;
	std::stringstream ss(chaine);
	std::string	sous_chaine;

	while(std::getline(ss, sous_chaine, delimiteur))
	{
		v.push_back(sous_chaine);
	}
	return (v);
}

bool safe_atoi(const char *str, int &result)
{
	int sign = 1;
	int value = 0;

	if (!str)
		return false;

	while ((*str >= 9 && *str <= 13) || *str == ' ')
		str++;

	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}

	if (*str < '0' || *str > '9')
		return false;

	// Parse digits
	while (*str >= '0' && *str <= '9')
	{
		int digit = *str - '0';

		// Overflow check
		if (sign == 1)
		{
			if (value > (INT_MAX - digit) / 10)
				return false; // overflow
		}
		else
		{
			if (value > (-(INT_MIN + digit)) / 10)
				return false; // underflow
		}

		value = value * 10 + digit;
		str++;
	}

	result = value * sign;
	return true;
}

HTTPRequest::HTTPRequest(std::string &buffer, const Server& serv)
{
	std::stringstream ss(buffer);
	std::string line;
	std::getline(ss, line);

	std::vector<std::string> firstline = split(line, ' ');
	if (firstline.size() < 3)
		throw HTTPRequest::HTTPRequestException("HTTP/1.1,400,Bad Request");

	std::string target = firstline[1];
	std::string version = firstline[2];

	// Commence par version pour pouvoir le throw si je dois
	if (!version.empty() && version.back() == '\r')// nettoyage du \r final
		version.pop_back();

	m_version = version;
	if (m_version != "HTTP/1.0" && m_version != "HTTP/1.1")
		throw HTTPRequest::HTTPRequestException("HTTP/1.1,501,Not Implemented");


	// séparateur query
	size_t pos = target.find('?');
	if (pos != std::string::npos)
	{		//separe le maillon avant/apres "?"
		m_target = target.substr(0, pos);
		query_creation(line);
	}
	else
		m_target = target;

	if (m_target.empty())
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

	while (std::getline(ss, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.empty())
			break;

	// chercher le séparateur clé/valeur
		size_t pos = line.find(':');
		if (pos == std::string::npos)
			throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

		std::string key = line.substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		std::string value = line.substr(pos + 1);

		// trim espace(s) au début de la valeur
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		// gerer headers multiples (concat avec ", ")
		if (m_headers.count(key))
			m_headers[key] += ", " + value;
		else
			m_headers[key] = value;
	}
	// Verification des headers

	if (m_headers.count("transfer-encoding"))
	{
		if (this->m_headers["transfer-encoding"] == "chunked" && this->m_version == "HTTP/1.0")
		{
			throw HTTPRequest::HTTPRequestException(m_version + "501 NotImplemented");
		}
	}

	if (m_headers.count("transfer-encoding") && m_headers.count("content-length"))
	{
		throw HTTPRequest::HTTPRequestException(m_version+",400,Bad Request");
	}

	if (!m_headers.count("connection"))
	{

		if (this->m_version == "HTTP/1.0")
		{
			this->m_headers["connection"] = "keep-alive";
		}
		else
			this->m_headers["connection"] = "close";
	}
	else
	{
		if (this->m_headers["connection"] != "keep-alive" && this->m_headers["connection"] != "close")
		{
			throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
		}
	}

	if (!m_headers.count("host") && m_version == "HTTP/1.1")
	{
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
	}

	// body
	const std::string delimiter = "\r\n\r\n";
	size_t headerEnd = buffer.find(delimiter);

	if (headerEnd == std::string::npos)
	{
		throw HTTPRequest::HTTPRequestException("restart getbuffer");
	}

	// No body expected if no Content-Length
	if (!m_headers.count("content-length"))
	{
		return;
	}

	int contentLength = 0;
	if (!safe_atoi(m_headers["content-length"].c_str(), contentLength) || contentLength < 0)
	{
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
	}

	// Optional max body size check
	// if (contentLength > MAX_BODY_SIZE)
	// 	throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

	size_t bodyStart = headerEnd + delimiter.length();

	// Wait until full body is received
	if (buffer.size() < bodyStart + static_cast<size_t>(contentLength))
	{
		throw HTTPRequest::HTTPRequestException("restart getbuffer");
	}

	m_body = buffer.substr(bodyStart, contentLength);

}

void HTTPRequest::query_creation(std::string line)
{
	std::stringstream ss(line);
	std::string querys;

	while (std::getline(ss, querys, '&'))
	{
		size_t pos = querys.find('=');

		std::string key;
		std::string val;

		if (pos != std::string::npos)
		{
			key = querys.substr(0, pos);
			val = querys.substr(pos + 1);
		}
		else
		{
			// paramètre sans valeur (ex: ?debug)
			key = querys;
			val = "";
		}

		this->m_query[key] = val;
	}
}

HTTPRequest::~HTTPRequest()
{
}


std::string HTTPRequest::GetTarget() const
{
	return (this->m_target);
}

std::map<std::string, std::string> HTTPRequest::GetQuery() const
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

bool HTTPRequest::IsKeepAlive()
{
    if (this->m_headers["connection"] == "keep-alive")
		return true;
	return false;
}

HTTPRequest::HTTPRequestException::HTTPRequestException(std::string err) throw()
{
}

const char *HTTPRequest::HTTPRequestException::what() const throw()
{
	return (this->_err.c_str());
}

HTTPRequest::HTTPRequestException::~HTTPRequestException() throw()
{
}
