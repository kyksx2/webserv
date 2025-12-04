/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:37:27 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/04 13:13:13 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define  HTTPREQUEST_HPP

#include "HTTPResponse.hpp"

class HTTPRequest
{
	protected :
		std::string	m_target;
		std::string	m_query;
		std::string	m_version;
		std::map<std::string, std::string>	m_headers;

	public :
		HTTPRequest(std::string target,
				std::string query,
				std::string version,
				std::map<std::string, std::string> headers);
		virtual ~HTTPRequest();
		std::string	GetTarget() const;
		std::string	GetQuery() const;
		std::string	GetVersion() const;
		std::map<std::string, std::string>	GetHeaders() const;
		virtual	HTTPResponse	generateResponse() = 0;

		class	HTTPRequestExcpetion : public std::exception
		{
			public :
				HTTPRequestExcpetion() throw();
				virtual const char	*what() const throw();
				virtual ~HTTPRequestExcpetion() throw();
		};
};
#endif