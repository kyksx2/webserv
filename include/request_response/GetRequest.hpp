/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:46:05 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/24 16:07:40 by marvin           ###   ########.fr       */
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
};

std::string getMIME_Type(const std::string& target);
#endif
