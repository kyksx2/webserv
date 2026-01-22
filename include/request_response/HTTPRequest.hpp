/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:37:27 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/22 14:48:12 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define  HTTPREQUEST_HPP

#include "HTTPResponse.hpp"
#include "serv_handler/Server.hpp"

//! IL FAUT PEUT ETRE VERIFIE LES HEADERS OBLIGATOIRE DANa CHAQUE METHODE

class HTTPRequest
{
	protected :
		Server	m_serv;
		std::string	m_target;
		std::map<std::string, std::string>	m_query;
		std::string	m_version;
		std::map<std::string, std::string>	m_headers;
		std::string	m_body;

	public :
		HTTPRequest(std::string &buffer, const Server& serv);
		HTTPRequest& operator=(const HTTPRequest& src);
		virtual ~HTTPRequest();


		void SetBody(std::string &buffer);

		std::string							GetTarget() const;
		std::map<std::string, std::string>	GetQuery() const;
		std::string							GetVersion() const;
		std::string							GetBody() const;
		std::map<std::string, std::string>	GetHeaders() const;
		std::string		GetHeaders_value(std::string key);

		void query_creation(std::string line);
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

std::ostream& operator<<(std::ostream& os, const HTTPRequest& req);
std::vector<std::string>	split(const std::string &chaine, char delimiteur);
bool						safe_atoi(const char *str, int &result);

#endif