/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:45:23 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/21 15:40:19 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::string &buffer, const Server& serv)
{
	std::stringstream ss(buffer);
	std::string line;
	std::getline(ss, line);

	this->m_serv = serv;
	std::vector<std::string> firstline = split(line, ' ');
	if (firstline.size() < 3)
		throw HTTPRequest::HTTPRequestException("HTTP/1.1,400,Bad Request");

	std::string method = firstline[0];
	std::string target = firstline[1];
	std::string version = firstline[2];

	// nettoyage du \r final sur la version
	if (!version.empty() && version[version.length() - 1] == '\r')
		version.erase(version.length() - 1);

	m_version = version;
	if (m_version != "HTTP/1.0" && m_version != "HTTP/1.1")
		throw HTTPRequest::HTTPRequestException("HTTP/1.1,501,Not Implemented");

	// validation méthode minimale
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw HTTPRequest::HTTPRequestException(m_version + ",405,Method Not Allowed");

	// séparateur query
	size_t qpos = target.find('?');
	if (qpos != std::string::npos)
	{
		m_target = target.substr(0, qpos);
		query_creation(target.substr(qpos + 1));
	}
	else
		m_target = target;

	if (m_target.empty())
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

	while (std::getline(ss, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		if (line.empty())
			break;

		// séparateur clé/valeur
		size_t pos = line.find(':');
		if (pos == std::string::npos)
			throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

		std::string key = line.substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		std::string value = line.substr(pos + 1);

		// trim espaces au début de la valeur
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		// headers multiples
		if (m_headers.count(key))
			m_headers[key] += ", " + value;
		else
			m_headers[key] = value;
	}

	// Transfer-Encoding + HTTP/1.0
	if (m_headers.count("transfer-encoding") && m_headers.count("content-length"))
	{
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
	}

	// Transfer-Encoding + Content-Length interdits ensemble
	if (m_headers.count("transfer-encoding") && m_headers.count("content-length"))
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

	// Connection par défaut (corrigé)
	if (!m_headers.count("connection"))
	{
		if (m_version == "HTTP/1.1")
			m_headers["connection"] = "keep-alive";
		else
			m_headers["connection"] = "close";
	}
	else
	{
		if (m_headers["connection"] != "keep-alive" &&
			m_headers["connection"] != "close")
			throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
	}
	// Host obligatoire en HTTP/1.1
	if (!m_headers.count("host") && m_version == "HTTP/1.1")
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &src)
{
	if (this == &src)
		return (*this);
	this->m_serv = src.m_serv;
	this->m_target = src.m_target;
	this->m_query = src.m_query;
	this->m_version = src.m_version;
	this->m_headers = src.m_headers;
	this->m_body = src.m_body;
	return *this;
}

HTTPRequest::~HTTPRequest()
{
}

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

void HTTPRequest::SetBody(std::string &buffer)
{
	// Pas de body attendu
	if (!m_headers.count("content-length"))
		return;

	int contentLength = 0;
	if (!safe_atoi(m_headers["content-length"].c_str(), contentLength) || contentLength < 0)
		throw HTTPRequestException(m_version + ",400,Bad Request");

	if (buffer.size() < static_cast<size_t>(contentLength))
		return; // body incomplet → PAS une erreur

	m_body = buffer.substr(0, contentLength);
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

std::string HTTPRequest::GetHeaders_value(std::string key)
{
	std::string	val;

	if (this->m_headers.count(key))
	{
		val = this->m_headers[key];
	}
	return val;
}

HTTPRequest::HTTPRequestException::HTTPRequestException(std::string err) throw()
{
	this->_err = err;
}

const char *HTTPRequest::HTTPRequestException::what() const throw()
{
	return (this->_err.c_str());
}

HTTPRequest::HTTPRequestException::~HTTPRequestException() throw()
{
}

std::ostream& operator<<(std::ostream& os, const HTTPRequest& req)
{
	os << "===== HTTPRequest =====" << std::endl;

	os << "Target  : " << req.GetTarget() << std::endl;
	os << "Version : " << req.GetVersion() << std::endl;

	os << "--- Query ---" << std::endl;
	std::map<std::string, std::string> query = req.GetQuery();
	for (std::map<std::string, std::string>::const_iterator it = query.begin();
		it != query.end(); ++it)
	{
		os << it->first << " = " << it->second << std::endl;
	}

	os << "--- Headers ---" << std::endl;
	std::map<std::string, std::string> headers = req.GetHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		it != headers.end(); ++it)
	{
		os << it->first << ": " << it->second << std::endl;
	}

	os << "--- Body ---" << std::endl;
	os << req.GetBody() << std::endl;

	os << "=======================" << std::endl;
	return os;
}
