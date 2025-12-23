/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/23 12:47:09 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/PostRequest.hpp"

PostRequest::PostRequest(std::string &buffer, const Server& serv)
: HTTPRequest(buffer, serv)
{
}

PostRequest::~PostRequest()
{
}

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

//! Pour POST avec 201 Created, Location si body contient URI → 500 si absent (optionnel selon ton serveur)
HTTPResponse PostRequest::generateResponse()
{
	HTTPResponse	postresponse;
	postresponse.setVersion(this->m_version);
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
					// 200 OK
					postresponse.setStatus(200, "OK");
					postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
					monFlux << this->m_body;
					monFlux.close();
					return (postresponse);
				}
				else
				{
					// 500 Internal Server Error
					postresponse.setStatus(500, "Internal Server Error");
					return (postresponse);
				}
			}
			else
			{
				// 403 Forbidden
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
					// 200 OK
					postresponse.setStatus(200, "OK");
					postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
					monFlux << this->m_body;
					monFlux.close();
					return (postresponse);
				}
				else
				{
					// 500 Internal Server Error
					postresponse.setStatus(500, "Internal Server Error");
					return (postresponse);
				}
			}
			else
			{
				// 403 Forbidden
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
						// 200 OK
						postresponse.setStatus(200, "OK");
						postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
						monFlux << this->m_body;
						monFlux.close();
						return (postresponse);
					}
					else
					{
							// 500 Internal Server Error
							postresponse.setStatus(500, "Internal Server Error");
							return (postresponse);
					}
				}
				else
				{
					// 403 Forbidden
					postresponse.setStatus(403, "Forbidden");
					return (postresponse);
				}
			}
			else
			{
				// 403 Forbidden
				postresponse.setStatus(403, "Forbidden");
				return (postresponse);
			}
		}
		else
		{
			// 404 Not Found
			postresponse.setStatus(404, "Not Found");
			return (postresponse);
		}
	}
}
