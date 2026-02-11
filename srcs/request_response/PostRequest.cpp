/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:54 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/11 15:26:09 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/PostRequest.hpp"
#include "serv_handler/Client.hpp"
#include "serv_handler/Webserv.hpp"

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
	std::string		realPath = this->GetRealPath();
	struct stat st;
	std::string	dir;
	std::string	file;
	split_path(this->m_target, dir, file);

	// Verifie si la Methode est autorise sur target
	if (this->m_location->isMethodAllowed("POST") == false)
	{
		// → 405 Method Not Allowed
		postresponse.setStatus(405, "Method Not Allowed");
		std::string method_allowed = vstos(this->m_location->getAllowedMethods(), ", ");
		postresponse.setHeader("Allow", method_allowed);
		return postresponse;
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

void PostRequest::startCgi(int epoll_fd, std::map<int, Client*>& client_map, Client* client)
{
	// (void)epoll_fd;
	// (void)client_map;
	// (void)client;
	client->setStartCgi(time(NULL));
	char **env = this->generateEnvp();
	int pipe_to_cgi[2];
	int pipe_from_cgi[2];
	pid_t pid = 0;

	if (pipe(pipe_to_cgi) == -1 || pipe(pipe_from_cgi) == -1) {
		//? return une erreur 500
	}
	pid = fork();
	if  (pid == -1) {
		close(pipe_to_cgi[0]);
		close(pipe_to_cgi[1]);
		close(pipe_from_cgi[0]);
		close(pipe_from_cgi[1]);
		//? return une erreur 500
	}
	else if (pid == 0) { //! child -> oubie qu'il est un serveur et execute le script
		//? ecrit dans [1](write) et lis dans [0](read)
		//! double tableau pour execve + recuperation de l'env
		//! ecrire dans file_from_cgi[1]
		//? dup stdin dans l'ecriture de pipe_from_cgi[1];
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGPIPE, SIG_DFL);

		//! gerer les signaux en les remettant comme ils etaient definis de base
		dup2(pipe_to_cgi[0], STDIN_FILENO); //? en cas de POST on a des donnees a recup
		dup2(pipe_from_cgi[1], STDOUT_FILENO); //? ecrit ici pour que le parent puisse read
		close(pipe_to_cgi[0]);
		close(pipe_to_cgi[1]);
		close(pipe_from_cgi[0]);
		close(pipe_from_cgi[1]);

		const std::map<std::string, std::string> cgi_h = this->m_location->getCgiHandlers();
		std::map<std::string, std::string>::const_iterator it = cgi_h.find(this->GetExtension());
		std::string _bin = (it != cgi_h.end()) ? it->second : "";
		std::string _script = this->GetRealPath();

		char *args[3];
		args[0] = (char *) _bin.c_str();
		args[1] = (char *) _script.c_str();
		args[2] = NULL;
		for (int i = 3; i < 1024; i++)
			close(i);
		if (execve(args[0], args, env) == -1) {
			if (env) {
				for(int i = 0; env[i]; i++) {
					free(env[i]);
				}
				delete[] env;
			}			//? return une erreur 500, le script ne sait pas executer + free
			perror("execve error");
			exit(1);
		}
	}
	if (env) {
		for(int i = 0; env[i]; i++) {
			free(env[i]);
		}
		delete[] env;
	}
	//! parent ici -> erit dans la pipe_to_cgi[1]
	//!            -> lis dans pipe_from_cgi[0]
	close(pipe_to_cgi[0]);
	close(pipe_from_cgi[1]);
	
	fcntl(pipe_from_cgi[0], F_SETFL, O_NONBLOCK);
	fcntl(pipe_from_cgi[0], F_SETFD, FD_CLOEXEC);

	std::string body = this->GetBody();
    if (!body.empty())
        write(pipe_to_cgi[1], body.c_str(), body.size());
    close(pipe_to_cgi[1]);

	client->setCgiStatus(true);
	client->setCgiFd(pipe_from_cgi[0]);
	client->setCgiPid(pid);

	struct epoll_event event;
	event.data.fd = pipe_from_cgi[0];
	event.events = EPOLLIN;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_from_cgi[0], &event);
	client_map[pipe_from_cgi[0]] = client;
}
