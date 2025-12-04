/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:59:01 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/04 14:38:15 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
{
}

HTTPResponse::~HTTPResponse()
{
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
