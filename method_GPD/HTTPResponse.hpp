/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:52:24 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/04 14:35:56 by yzeghari         ###   ########.fr       */
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

enum HTTPStatusCode {
	OK = 200,
	Created = 201,
	Accepted = 202,
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
		int	_status_code;
		std::string	_reason_phrase;
		std::map<std::string, std::string>	_headers;
		std::string	_body;

	public:
		HTTPResponse();
		~HTTPResponse();
		void	setStatus(int status_code, std::string reason_phrase);
		void	setHeader(std::string key, std::string value);
		void	setBody(std::string	body);
		// generate;
};

#endif