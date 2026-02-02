/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:46:05 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/02 12:56:56 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETREQUEST_HPP
# define  GETREQUEST_HPP

#include "HTTPRequest.hpp"

class GetRequest : public HTTPRequest
{
	public:
		GetRequest(std::string &buffer, const Server& serv);
		~GetRequest();
		HTTPResponse	generateResponse();
		char			**generateEnvp();
		std::string			generateCGIResponse();
};

std::string getMIME_Type(const std::string& target);
#endif
