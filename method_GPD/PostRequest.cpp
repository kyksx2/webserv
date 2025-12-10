/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/10 16:47:46 by yzeghari         ###   ########.fr       */
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

//! A ranger
void	split_path(std::string path, std::string &dir, std::string &file)
{
	std::string::size_type pos = path.find_last_of("/\\");

	if (pos == std::string::npos)
	{
		dir = "";
		file = path;
	}
	else
	{
		dir = path.substr(0, pos);
		file = path.substr(pos + 1);
	}
}

HTTPResponse PostRequest::generateResponse()
{
	HTTPResponse	postresponse;
	struct stat st;
	std::string	dir;
	std::string	file;
	split_path(this->m_target, dir, file);

	//! verif PERM methode
	if (stat(this->m_target.c_str(), &st) == 0)
	{
		if (S_ISREG(st.st_mode))
		{
			if (access(this->m_target.c_str(), W_OK) == 0)
			{
				// 200 OK ou 204 (overwrite)
			}
			else
			{
				//perm no=403
			}
		}
		else if (S_ISDIR(st.st_mode))
		{
			//verif perm no=403
			if (access(this->m_target.c_str(), W_OK) == 0)
			{

			}
			else
			{
			//perm no=403
			}
		}
	}
	else
	{
		if (stat(dir.c_str(), &st) == 0)
		{
			if (S_ISDIR(st.st_mode))
			{
				if (access(dir.c_str(), W_OK) == 0)
				{
					//201 CREATED
				}
				else
				{
					//perm no=403
				}
			}
			else if (S_ISREG(st.st_mode))
			{
				// 403 Forbidden
			}
		}
		else
		{
			// → 404 Not Found
		}
	}
	return ;
}
