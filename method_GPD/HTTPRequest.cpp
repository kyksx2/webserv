/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:45:23 by yzeghari          #+#    #+#             */
/*   Updated: 2025/11/25 16:38:16 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{
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
