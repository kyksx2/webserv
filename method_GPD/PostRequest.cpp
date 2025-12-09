/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/09 16:56:02 by yzeghari         ###   ########.fr       */
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

HTTPResponse PostRequest::generateResponse()
{
	HTTPResponse	postresponse;
	struct stat st;

	//! verif PERM methode

	if (!stat(this->m_target.c_str(), &st))
	{
		if (S_ISREG(st.st_mode))
		{
			if (access())
			{
				//perm no=403
			}
			else
			{
				// 201 Created
			}
		}
		else if (S_ISDIR(st.st_mode))
		{
			//verif perm no=403
			if (access())
		}
	}
	else
	{
		//  201 Created
	}
	return ;
}
