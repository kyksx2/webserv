/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/27 17:01:55 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/PostRequest.hpp"

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
	postresponse.setHeader("connection", this->m_headers["connection"]);
	std::string		realPath = this->m_target;
	struct stat st;
	std::string	dir;
	std::string	file;
	split_path(this->m_target, dir, file);

	//! faire le menage une fois tt pret
	std::cout << "envp = \n" << std::endl;
	for (int i = 0; this->generateEnvp()[i] != NULL; i++)
	{
		std::cout << this->generateEnvp()[i] << std::endl;
	}
	const Location_config* location = this->m_serv.sendALocation(realPath);
	if (location)
	{
		realPath = location->getRoot() + this->m_target;
		if (location->isMethodAllowed("POST") == false)
		{
			postresponse.setStatus(405, "Method Not Allowed");
			std::string method_allowed = vstos(location->getAllowedMethods(), ", ");
			postresponse.setHeader("Allow", method_allowed);
			return postresponse;
		}
	}

	if (stat(realPath.c_str(), &st) == 0)
	{
		if (S_ISREG(st.st_mode))
		{
			std::ofstream monFlux(realPath.c_str(), std::ios::out | std::ios::trunc);
			if (monFlux)
			{
				postresponse.setStatus(200, "OK");
				postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
				monFlux << this->m_body;
				monFlux.close();
				return postresponse;
			}
			else
			{
				postresponse.setStatus(500, "Internal Server Error");
				return postresponse;
			}
		}
		else if (S_ISDIR(st.st_mode))
		{
			std::ofstream monFlux((realPath + "/default").c_str(), std::ios::out | std::ios::trunc);
			if (monFlux)
			{
				postresponse.setStatus(200, "OK");
				postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
				monFlux << this->m_body;
				monFlux.close();
				return postresponse;
			}
			else
			{
				postresponse.setStatus(500, "Internal Server Error");
				return postresponse;
			}
		}
	}
	else
	{
		if (stat(dir.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
		{
			std::ofstream monFlux(realPath.c_str(), std::ios::out | std::ios::trunc);
			if (monFlux)
			{
				postresponse.setStatus(200, "OK");
				postresponse.setHeader("Content-Type", this->m_headers["content-type"]);
				monFlux << this->m_body;
				monFlux.close();
				return postresponse;
			}
			else
			{
				postresponse.setStatus(500, "Internal Server Error");
				return postresponse;
			}
		}
		else
		{
			postresponse.setStatus(404, "Not Found");
			return postresponse;
		}
	}
	return postresponse;
}

char **PostRequest::generateEnvp()
{
	// (stdin = body)
	std::vector<std::string> env;

	env.push_back("REQUEST_METHOD=POST");

	env.push_back(
		"CONTENT_LENGTH=" +
		(this->m_headers.count("content-length")
			? this->m_headers["content-length"]
			: "0")
	);

	env.push_back(
		"CONTENT_TYPE=" +
		(this->m_headers.count("content-type")
			? this->m_headers["content-type"]
			: "")
	);

	env.push_back("QUERY_STRING=" + (this->m_query.empty() ? "" : this->m_query));
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

