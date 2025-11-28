/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 12:27:58 by yzeghari          #+#    #+#             */
/*   Updated: 2025/11/28 12:33:39 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteRequest.hpp"

DeleteRequest::DeleteRequest(std::string target, std::string query, std::string version, std::map<std::string, std::string> headers)
: HTTPRequest(target, query, version, headers)
{
}

DeleteRequest::~DeleteRequest()
{
}

std::string DeleteRequest::generateResponse()
{
	return std::string();
}
