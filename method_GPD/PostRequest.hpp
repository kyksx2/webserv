/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:39:08 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/08 14:42:26 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTRequest_HPP
# define  POSTRequest_HPP

#include "HTTPRequest.hpp"

class PostRequest : public HTTPRequest
{
	public:
		PostRequest(std::string target,
				std::string query,
				std::string version,
				std::map<std::string, std::string> headers);
		~PostRequest();
		HTTPResponse	generateResponse();
};
#endif