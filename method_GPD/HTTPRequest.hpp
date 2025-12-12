/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:37:27 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/12 15:46:47 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define  HTTPREQUEST_HPP

#include "HTTPResponse.hpp"

//! la recuperation des config du serv, besoin pour les methodes

//! IL FAUT PEUT ETRE VERIFIE LES HEADERS OBLIGATOIRE SOIT ICI SOIT DANC CHAQUE METHODE

class HTTPRequest
{
	protected :
		std::string	m_target;
		std::string	m_query;
		std::string	m_version;
		std::map<std::string, std::string>	m_headers;
		std::string	m_body;

	public :
		HTTPRequest(std::vector<std::string> &v);
		virtual ~HTTPRequest();
		std::string	GetTarget() const;
		std::string	GetQuery() const;
		std::string	GetVersion() const;
		std::string	GetBody() const;
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