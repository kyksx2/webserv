/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:59:01 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/06 14:45:59 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/HTTPResponse.hpp"

HTTPResponse::HTTPResponse() : _version("HTTP/1.1"), _status_code(400), _reason_phrase("Bad Request") {} //!! mettre quelque chose par default

HTTPResponse::HTTPResponse(std::string version, std::string buffer)
{
	this->_version = version;

	std::cout << "======BUFFER RECU======\n" << buffer << "======================="<< std::endl;
	// Status | par defaut 200 OK
	this->_status_code = 200;
	this->_reason_phrase = "OK";
	// Headers
	std::stringstream	ss(buffer);
	std::string	line;
	while (std::getline(ss, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		if (line.empty())
			break;

		// séparateur clé/valeur
		size_t pos = line.find(':');
		if (pos == std::string::npos)
			continue;

		std::string key = line.substr(0, pos);

		// Status specifie
		if (key == "Status")
		{
			std::vector<std::string> statusline = split(line, ' ');

			this->_status_code = std::atoi(statusline[1].c_str());

			size_t first_space = line.find(' ');
			size_t second_space = line.find(' ', first_space + 1);
			if (second_space != std::string::npos)
				_reason_phrase = line.substr(second_space + 1);

			continue;
		}

		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		std::string value = line.substr(pos + 1);

		// trim espaces au début de la valeur
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		// headers multiples
		if (_headers.count(key))
			_headers[key] += ", " + value;
		else
			_headers[key] = value;
	}

	// Content-Length sera cree plus tard dans generate() | a supprimer si existe deja
	_headers.erase("content-length");

	size_t pos_body = buffer.find("\r\n\r\n");
	size_t sep_len = 4;

	if (pos_body == std::string::npos)
	{
		pos_body = buffer.find("\n\n");
		sep_len = 2;
	}

	if (pos_body != std::string::npos)
		_body = buffer.substr(pos_body + sep_len);
}

HTTPResponse::HTTPResponse(std::string version, int status_code, std::string reason_phrase)
{
	this->_version = version;
	this->_status_code = status_code;
	this->_reason_phrase = reason_phrase;
	this->_body = reason_phrase;
}

HTTPResponse &HTTPResponse::operator=(const HTTPResponse &src)
{
	if (this == &src)
		return (*this);
	this->_version = src._version;
	this->_status_code = src._status_code;
	this->_reason_phrase = src._reason_phrase;
	this->_headers = src._headers;
	this->_body = src._body;
	return *this;
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::setVersion(std::string version)
{
	this->_version = version;
}

void HTTPResponse::setStatus(int status_code, std::string reason_phrase)
{
	this->_status_code = status_code;
	this->_reason_phrase = reason_phrase;
}

void HTTPResponse::setHeader(std::string key, std::string value)
{
	this->_headers[key] = value;
}

void HTTPResponse::setBody(std::string body)
{
	this->_body = body;
}

std::string HTTPResponse::GetVersion() const
{
	return (this->_version);
}

int HTTPResponse::GetStatusCode() const
{
	return (this->_status_code);
}

std::string HTTPResponse::GetReason() const
{
	return (this->_reason_phrase);
}

std::map<std::string, std::string> HTTPResponse::GetHeaders() const
{
	return (this->_headers);
}

std::string HTTPResponse::GetBody() const
{
	return (this->_body);
}
\
bool HTTPResponse::IsKeepAlive()
{
	if (this->_headers.count("connection"))
	{
		if (this->_headers["connection"] == "keep-alive")
			return true;
		return false;
	}
	return false; // au cas ou ??
}

std::string HTTPResponse::generate()
{
	std::ostringstream	response;

	response << "HTTP/1.1 "
	<< this->_status_code << " "
	<< this->_reason_phrase << "\r\n";

	if (this->_body.empty())
		_body = _reason_phrase;

	if (this->_status_code == 204 || this->_status_code == 304)
		_body = "";

	//ptet a mttre en minuscule pour debug
	std::ostringstream oss;
	oss << _body.size();
	_headers["Content-Length"] = oss.str();
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
		it != _headers.end();
		++it)
	{
		response << it->first << ": " << it->second << "\r\n";
	}
	response << "\r\n";
	response << this->_body << std::endl;
	return response.str();
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

std::ostream& operator<<(std::ostream& os, const HTTPResponse& res)
{
	os << "===== HTTPResponse =====" << std::endl;

	os << "Version : " << res.GetVersion() << std::endl;
	os << "Status Code : " << res.GetStatusCode() << std::endl;
	os << "Reason : " << res.GetReason() << std::endl;

	os << "--- Headers ---" << std::endl;
	std::map<std::string, std::string> headers = res.GetHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		it != headers.end(); ++it)
	{
		os << it->first << ": " << it->second << std::endl;
	}

	os << "--- Body ---" << std::endl;
	os << res.GetBody() << std::endl;

	os << "=======================" << std::endl;
	return os;
}