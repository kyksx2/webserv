/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:59:01 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/18 16:51:38 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
{
}

HTTPResponse::HTTPResponse(std::string version, int status_code, std::string reason_phrase)
{
	this->_version = version;
	this->_status_code = status_code;
	this->_reason_phrase = reason_phrase;

	this->_body = reason_phrase;
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

std::string	HTTPResponse::generate()
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
	_headers["Content-Length"] = _body.size();
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
		it != _headers.end();
		++it)
	{
		response << it->first << ": " << it->second << "\r\n";
	}
	response << "\r\n";
	response << this->_body;
	return response.str();
}