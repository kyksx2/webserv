/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/18 17:06:53 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostRequest.hpp"

//! Pour POST avec 201 Created, Location si body contient URI → 500 si absent (optionnel selon ton serveur)
PostRequest::PostRequest(std::string &buffer, const Server& serv)
: HTTPRequest(buffer, serv)
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
				std::ofstream	monFlux(this->m_target.c_str(), std::ios::out | std::ios::trunc);
				if (monFlux)
				{
					//200
					postresponse.setStatus(200, "OK");
					postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
					monFlux << this->m_body;
					monFlux.close();
					return (postresponse);
				}
				else
				{
					// 500
					postresponse.setStatus(500, "Internal Server Error");
					return (postresponse);
				}
			}
			else
			{
				//403
				postresponse.setStatus(403, "Forbidden");
				return (postresponse);
			}
		}
		else if (S_ISDIR(st.st_mode))
		{
			if (access(this->m_target.c_str(), W_OK) == 0)
			{
				//! A voir si il faut rajouter la bonne extension
				std::ofstream	monFlux((this->m_target + "default").c_str(), std::ios::out | std::ios::trunc);
				if (monFlux)
				{
					//200
					postresponse.setStatus(200, "OK");
					postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
					monFlux << this->m_body;
					monFlux.close();
					return (postresponse);
				}
				else
				{
					// 500
					postresponse.setStatus(500, "Internal Server Error");
					return (postresponse);
				}
			}
			else
			{
				//403
				postresponse.setStatus(403, "Forbidden");
				return (postresponse);
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

					std::ofstream	monFlux(this->m_target.c_str(), std::ios::out | std::ios::trunc);
					if (monFlux)
					{
						//200
						postresponse.setStatus(200, "OK");
						postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
						monFlux << this->m_body;
						monFlux.close();
						return (postresponse);
					}
					else
					{
							//500
							postresponse.setStatus(500, "Internal Server Error");
							return (postresponse);
					}
				}
				else
				{
					//403
					postresponse.setStatus(403, "Forbidden");
					return (postresponse);
				}
			}
			else
			{
				//403
				postresponse.setStatus(403, "Forbidden");
				return (postresponse);
			}
		}
		else
		{
			//404 Not Found
			postresponse.setStatus(404, "Not Found");
			return (postresponse);
		}
	}
}
