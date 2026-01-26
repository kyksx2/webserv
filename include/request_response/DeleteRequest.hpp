/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:38:21 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/26 16:31:36 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETERequest_HPP
# define  DELETERequest_HPP

#include "HTTPRequest.hpp"

class DeleteRequest : public HTTPRequest
{
	public:
		DeleteRequest(std::string &buffer, const Server& serv);
		~DeleteRequest();
		HTTPResponse	generateResponse();
		char			**generateEnvp();
};

#endif