/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:37:27 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/02 10:32:40 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define  HTTPREQUEST_HPP

#include "HTTPResponse.hpp"
#include "serv_handler/Server.hpp"
// #include "CGI/CgiHandler.hpp"

//! Liste des chose a faire : Max body size, .Env, utilisation du Query si needed, chunked

class HTTPRequest
{
	protected :
		Server	m_serv;
		std::string	m_target;
		std::string	m_query;
		std::string	m_version;
		std::map<std::string, std::string>	m_headers;
		std::string	m_body;

	public :
		HTTPRequest(std::string &buffer, const Server& serv);
		HTTPRequest& operator=(const HTTPRequest& src);
		virtual ~HTTPRequest();


		void SetBody(std::string &buffer);

		std::string							GetTarget() const;
		std::string							GetQuery() const;
		std::string							GetVersion() const;
		std::string							GetBody() const;
		std::map<std::string, std::string>	GetHeaders() const;
		std::string		GetHeaders_value(std::string key);

		virtual	HTTPResponse	generateResponse() = 0;
		// virtual	void			generateCGI() = 0; //? a faire
		virtual	char			**generateEnvp() = 0;

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
std::string vstos(const std::vector<std::string>& v, const std::string& separateur);
bool						safe_atoi(const char *str, int &result);

#endif
