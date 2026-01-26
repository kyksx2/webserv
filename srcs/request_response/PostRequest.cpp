/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/26 16:48:14 by yzeghari         ###   ########.fr       */
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

	std::string scriptPath =
		this->m_serv.sendALocation(this->m_target)->getRoot()
		+ this->m_target;

	env.push_back("SCRIPT_NAME=" + scriptPath);

	char **envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++)
		envp[i] = strdup(env[i].c_str());

	envp[env.size()] = NULL;
	return envp;
}

