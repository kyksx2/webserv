/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2025/11/28 12:33:26 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostRequest.hpp"

PostRequest::PostRequest(std::string target, std::string query, std::string version, std::map<std::string, std::string> headers)
: HTTPRequest(target, query, version, headers)
{
}

PostRequest::~PostRequest()
{
}

std::string PostRequest::generateResponse()
{
	return std::string();
}
