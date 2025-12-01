/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:45:13 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/01 15:45:51 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetRequest.hpp"

GetRequest::GetRequest(std::string target, std::string query, std::string version, std::map<std::string, std::string> headers)
: HTTPRequest(target, query, version, headers)
{
}

GetRequest::~GetRequest()
{
}

std::string GetRequest::generateResponse()
{
	

}
