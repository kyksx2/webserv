/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 12:27:58 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/02 16:29:35 by yzeghari         ###   ########.fr       */
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
	std::string		realPath = this->GetRealPath();
	struct stat st;

	// Verifie si la Methode est autorise sur target
	if (this->m_location->isMethodAllowed("DELETE") == false)
	{
		// → 405 Method Not Allowed
		delresponse.setStatus(405, "Method Not Allowed");
		this->m_location->getAllowedMethods();
		std::string method_allowed = vstos(this->m_location->getAllowedMethods(), ", ");
		delresponse.setHeader("Allow", method_allowed);
		return delresponse;
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

char **DeleteRequest::generateEnvp()
{
	std::vector<std::string> env;

	env.push_back("REQUEST_METHOD=DELETE");
	env.push_back("SERVER_PROTOCOL=" + this->m_version);

	// ===== SCRIPT_NAME (URI, pas filesystem) =====
	std::string script_uri = this->m_target;
	size_t pos = script_uri.find(".cgi");
	if (pos != std::string::npos)
		script_uri = script_uri.substr(0, pos + 4);

	env.push_back("SCRIPT_NAME=" + script_uri);

	// ===== Server config =====
	const Server_Config sv_c = m_serv.getConfig();

	// ===== SERVER_NAME & SERVER_PORT =====
	std::string host = this->GetHeaders_value("host");
	std::string server_name;

	if (!host.empty())
	{
		size_t p = host.find(':');
		server_name = (p != std::string::npos) ? host.substr(0, p) : host;
	}
	else
	{
		server_name = sv_c.getServerNames().empty()
			? "localhost"
			: sv_c.getServerNames()[0];
	}

	env.push_back("SERVER_NAME=" + server_name);

	std::stringstream ss;
	ss << sv_c.getPort();
	std::string port = ss.str();

	env.push_back("SERVER_PORT=" + port);
	env.push_back("HTTP_HOST=" + host);

	// ===== PATH_INFO =====
	std::string path_info;
	if (pos != std::string::npos && pos + 4 < this->m_target.size())
		path_info = this->m_target.substr(pos + 4);

	env.push_back("PATH_INFO=" + path_info);

	// ===== PATH_TRANSLATED =====
	if (!path_info.empty())
	{
		std::string translated =
			this->m_serv.sendALocation(script_uri)->getRoot() + path_info;
		env.push_back("PATH_TRANSLATED=" + translated);
	}
	else
	{
		env.push_back("PATH_TRANSLATED=");
	}

	// ===== Allocation finale =====
	char **envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++)
		envp[i] = strdup(env[i].c_str());
	envp[env.size()] = NULL;

	return envp;
}
