/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 12:27:58 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/24 15:38:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/DeleteRequest.hpp"

DeleteRequest::DeleteRequest(std::string &buffer, const Server& serv)
: HTTPRequest(buffer, serv)
{
}

DeleteRequest::~DeleteRequest()
{
}

HTTPResponse DeleteRequest::generateResponse()
{
	HTTPResponse	delresponse;
	delresponse.setVersion(this->m_version);
	delresponse.setHeader("connection", this->m_headers["connection"]);
	std::string		realPath = this->m_target;
	struct stat st;

	// Si DELETE n'est pas autorisé dans ta config (location) A rajouter
	// → 405 Method Not Allowed
	const Location_config* location = this->m_serv.sendALocation(realPath);
	if (location) // theoriquement jms NULL | possibilité de changer pointeur en ref
	{
		realPath = location->getRoot() + this->m_target;
		if (location->isMethodAllowed("DELETE") == false)
		{
			delresponse.setStatus(405, "Method Not Allowed");
			location->getAllowedMethods();
			std::string method_allowed = vstos(location->getAllowedMethods(), ", ");
			delresponse.setHeader("Allow", method_allowed);
			return delresponse;
		}
	}

	if (!stat(realPath.c_str(), &st))
	{
		if (S_ISREG(st.st_mode))
		{
			if (!access(realPath.c_str(), W_OK))
			{
				if (unlink(realPath.c_str()) < 0)
				{
					if (errno == EACCES || errno == EPERM)
					{
						// → 403 Forbidden
						delresponse.setStatus(403, "Forbidden");
						return (delresponse);
					}
					else if (errno == ENOENT)
					{
						// 404 Not Found
						delresponse.setStatus(404, "Not Found");
						return (delresponse);
					}
					else
					{
						// 500 Internal Server Error
						delresponse.setStatus(500, "Internal Server Error");
						return (delresponse);
					}
				}
				else // cas reussite
				{
					// → 204 No Content
					delresponse.setStatus(204, "No Content");
					return (delresponse);
				}
			}
			else
			{
				// → 403 Forbidden
				delresponse.setStatus(403, "Forbidden");
				return (delresponse);
			}
		}
		else if (S_ISDIR(st.st_mode))
		{
			// → 403 Forbidden
			delresponse.setStatus(403, "Forbidden");
			return (delresponse);
		}
	}
	else
	{
		// 404 NOT FOUND
		delresponse.setStatus(404, "Not Found");
		return (delresponse);
	}
	return (delresponse);
}
