/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:59:01 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/21 11:33:57 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/HTTPResponse.hpp"
#include "request_response/HTTPResponse.hpp"

HTTPResponse::HTTPResponse() : _version("HTTP/1.1"), _status_code(200), _reason_phrase("OK") {} //!! mettre quelque chose par default

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
	response << this->_body;
	return response.str();
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