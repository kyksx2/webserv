/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:52:24 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/21 15:42:14 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <climits>
#include <algorithm>

enum HTTPStatusCode {
	OK = 200,
	Created = 201,
	Accepted = 202,
	NoContent = 204,
	MovedPermanently = 301,
	PermanentRedirect = 308,
	BadRequest = 400,
	Unauthorized = 401,
	Forbidden = 403,
	NotFound = 404,
	MethodNotAllowed = 405,
	RequestTimeout = 408,
	UnsupportedMediaType = 415,
	InternalServerError = 500,
	NotImplemented = 501
};

class HTTPResponse
{
 private:
	std::string	_version;
	int	_status_code;
	std::string	_reason_phrase;
	std::map<std::string, std::string>	_headers;
	std::string	_body;

 public:
	HTTPResponse();
	HTTPResponse(std::string version, int status_code, std::string reason_phrase);
	HTTPResponse& operator=(const HTTPResponse& src);
	~HTTPResponse();

	void	setVersion(std::string version);
	void	setStatus(int status_code, std::string reason_phrase);
	void	setHeader(std::string key, std::string value);
	void	setBody(std::string	body);

	std::string			GetVersion() const;
	int					GetStatusCode() const;
	std::string			GetReason() const;
	std::map<std::string, std::string>	GetHeaders() const;
	std::string			GetBody() const;
	bool 				IsKeepAlive();

	std::string		generate();
	//! pense a rajouter les headers obligatoire tel que lenght ou connection ds generate
};

std::ostream& operator<<(std::ostream& os, const HTTPResponse& res);
#endif