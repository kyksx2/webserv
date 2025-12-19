/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:37:27 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/19 17:00:09 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define  HTTPREQUEST_HPP

#include "HTTPResponse.hpp"

//! la recuperation des config du serv, besoin pour les methodes

//! IL FAUT PEUT ETRE VERIFIE LES HEADERS OBLIGATOIRE SOIT ICI SOIT DANC CHAQUE METHODE

struct Server;

class HTTPRequest
{
	protected :
		std::string	m_target;
		std::map<std::string, std::string>	m_query;
		std::string	m_version;
		std::map<std::string, std::string>	m_headers;
		std::string	m_body;

	public :
		HTTPRequest(std::string &buffer, const Server& serv);
		void query_creation(std::string line);
		virtual ~HTTPRequest();
		std::string	GetTarget() const;
		std::map<std::string, std::string>	GetQuery() const;
		std::string	GetVersion() const;
		std::string	GetBody() const;
		std::map<std::string, std::string>	GetHeaders() const;
		virtual	HTTPResponse	generateResponse() = 0;

		class	HTTPRequestException : public std::exception
		{
			private :
				std::string	_err;
			public :
				HTTPRequestException(std::string err) throw();
				virtual const char	*what() const throw();
				virtual ~HTTPRequestException() throw();
		};
};

std::vector<std::string>	split(const std::string &chaine, char delimiteur);
bool						safe_atoi(const char *str, int &result);
#endif