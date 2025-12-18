/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 12:27:58 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/18 14:52:21 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteRequest.hpp"

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
	struct stat st;

	// 5. Si DELETE n'est pas autorisé dans ta config (location) A rajouter
	// → 405 Method Not Allowed

	if (!stat(this->m_target.c_str(), &st))
	{
		if (S_ISREG(st.st_mode))
		{
			if (!access(this->m_target.c_str(), W_OK))
			{
				if (unlink(this->m_target.c_str()) < 0) // cas erreur
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

}
