/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:37:27 by yzeghari          #+#    #+#             */
/*   Updated: 2025/11/25 16:23:52 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define  HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <map>

#include <exception>
#include <vector>

class HTTPRequest
{
	protected :
		std::string	m_target;
		std::string	m_query;
		std::string	m_version;
		std::map<std::string, std::string>	m_headers;

	public :
		HTTPRequest();
		virtual ~HTTPRequest();
		std::string	GetTarget() const;
		std::string	GetQuery() const;
		std::string	GetVersion() const;
		std::map<std::string, std::string>	GetHeaders() const;
		virtual	std::string	generateResponse() = 0;

		class	HTTPRequestExcpetion : public std::exception
		{
			public :
				HTTPRequestExcpetion() throw();
				virtual const char	*what() const throw();
				virtual ~HTTPRequestExcpetion() throw();
		};
};
#endif