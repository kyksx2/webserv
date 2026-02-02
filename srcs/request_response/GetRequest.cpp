/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:45:13 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/02 16:18:56 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/GetRequest.hpp"

GetRequest::GetRequest(std::string &buffer, const Server& serv)
: HTTPRequest(buffer, serv)
{
}

GetRequest::~GetRequest()
{
}

// Trouve le header content type
std::string getMIME_Type(const std::string& target)
{
	std::string lst_ext[] = {".html", ".css", ".js", ".png", ".jpg"};
	std::string lst_MIME[] = {"text/html", "text/css", "application/javascript",
							"image/png", "image/jpeg"};

	int n = sizeof(lst_ext) / sizeof(lst_ext[0]);

	for (int i = 0; i < n; ++i)
	{
		if (target.size() >= lst_ext[i].size() &&
			target.compare(target.size() - lst_ext[i].size(), lst_ext[i].size(), lst_ext[i]) == 0)
		{
			return lst_MIME[i];
		}
	}

	// Par défaut, si aucune extension correspond, retourner text/html
	return "text/html";
}

HTTPResponse GetRequest::generateResponse()
{
	HTTPResponse	getresponse;
	getresponse.setVersion(this->m_version);
	getresponse.setHeader("connection", this->m_headers["connection"]);
	std::string	inthefile;
	//tester path../file/ != path../dir/
	std::string		realPath = this->GetRealPath();
	struct stat st;

	// Verifie si la Methode est autorise sur target
	if (this->m_location->isMethodAllowed("GET") == false)
	{
		// → 405 Method Not Allowed
		getresponse.setStatus(405, "Method Not Allowed");
		this->m_location->getAllowedMethods();
		std::string method_allowed = vstos(this->m_location->getAllowedMethods(), ", ");
		getresponse.setHeader("Allow", method_allowed);
		return getresponse;
	}
	if (this->m_target.find("..") != std::string::npos)
	{
		getresponse.setStatus(403, "Forbidden");
		return getresponse;
	}
	if (!stat(realPath.c_str(), &st)) // recupere le type du fichier
	{
		if (S_ISDIR(st.st_mode))
		{
			if (this->m_target.empty() || this->m_target[this->m_target.length() - 1] != '/'){
				// 301 redirect
				getresponse.setHeader("Location", this->m_target + "/");
				getresponse.setStatus(301, "Moved Permanently");
				std::string newLocation = this->m_target + "/";
				getresponse.setBody("<html><body><h1>301 Moved Permanently</h1>"
										"<p>Resource has moved to <a href=\"" + newLocation + "\">"
										+ newLocation + "</a></p></body></html>");
				return (getresponse);
			}
			else
			{
				std::string lst_index[] = {"index.html", "index.htm"};
				struct stat st_index;
				std::string	ntarget;

				for (int i = 0; i < 2; ++i)
				{
					ntarget = realPath + lst_index[i];
					if (!stat(ntarget.c_str(), &st_index))
					{
						std::ifstream	infile(ntarget.c_str());
						if (!infile)
						{
							// 403 forbidden
							getresponse.setStatus(403, "Forbidden");
							getresponse.setHeader("Content-Type", "text/html");
							getresponse.setBody(
								"<html><head><title>403 Forbidden</title></head>"
								"<body><h1>403 Forbidden</h1>"
								"<p>You don't have permission to access this resource.</p>"
								"</body></html>"
							);
							return (getresponse);
						}

						//recuperation du contenue fichier en brut code 200
						std::stringstream buffer;
						buffer << infile.rdbuf();
						inthefile = buffer.str();
						getresponse.setStatus(200, "OK");
						getresponse.setHeader("Content-Type", getMIME_Type(ntarget));
						getresponse.setBody(inthefile);
						return (getresponse);
					}
				}
				//cas auto-index
				if (this->m_location->isAutoindexEnabled())
				{
					//      yes     |  no
					// Generate list|   403

					// Listing Generation
					DIR	*fd_dir;
					fd_dir = opendir(realPath.c_str()); // ouvre le dossier et stock dans une struct
					if (!fd_dir)
					{
						getresponse.setStatus(200, "OK");
						getresponse.setHeader("Content-Type", "text/html");
						getresponse.setBody(
						"<!DOCTYPE html>"
						"<html>"
						"<head>"
							"<title>Directory listing</title>"
						"</head>"
						"<body>"
							"<h1>Index of this directory</h1>"
							"<p>Directory is empty.</p>"
						"</body>"
						"</html>");
						return (getresponse);
					}
					struct dirent* entry;
					while ((entry = readdir(fd_dir)) != NULL)
					{
						inthefile += entry->d_name;
						inthefile += "<br>";  // ou conserver \n et mettre tout dans <pre>
					}
					closedir(fd_dir);
					getresponse.setStatus(200, "OK");
					getresponse.setHeader("Content-Type", "text/html");
					getresponse.setBody(
						"<!DOCTYPE html>"
						"<html>"
						"<head>"
							"<title>Directory listing</title>"
						"</head>"
						"<body>"
							"<h1>Index of this directory</h1>"
							"<p>" + inthefile + "</p>"
						"</body>"
						"</html>"
					);
					return (getresponse);
				}
				else
				{
					// 403 forbidden
					getresponse.setStatus(403, "Forbidden");
					getresponse.setHeader("Content-Type", "text/html");
					getresponse.setBody(
						"<html><head><title>403 Forbidden</title></head>"
						"<body><h1>403 Forbidden</h1>"
						"<p>You don't have permission to access this resource.</p>"
						"</body></html>"
					);
					return (getresponse);
				}
			}
		}
		if (S_ISREG(st.st_mode))
		{
			//recuperer avec un ifstream si erreur return perm denied
			std::ifstream	infile(realPath.c_str());
			if (!infile)
			{
				// 403 forbidden
				getresponse.setStatus(403, "Forbidden");
				getresponse.setHeader("Content-Type", "text/html");
				getresponse.setBody(
					"<html><head><title>403 Forbidden</title></head>"
					"<body><h1>403 Forbidden</h1>"
					"<p>You don't have permission to access this resource.</p>"
					"</body></html>"
				);
				return (getresponse);
			}

			//recuperation du contenue fichier en brut code 200
			std::stringstream buffer;
			buffer << infile.rdbuf();
			inthefile = buffer.str();
			getresponse.setStatus(200, "OK");
			getresponse.setHeader("Content-Type", getMIME_Type(this->m_target));
			getresponse.setBody(inthefile);
			return (getresponse);
		}
	}
	else
	{
		// 404 Not found
		getresponse.setStatus(404, "Not Found");
		getresponse.setHeader("Content-Type", "text/html");
		getresponse.setBody(
			"<html><head><title>404 Not Found</title></head>"
			"<body><h1>404 Not Found</h1>"
			"<p>The requested resource '" + this->m_target + "' was not found on this server.</p>"
			"</body></html>"
		);
		return(getresponse);
	}
	return (getresponse);
}

char **GetRequest::generateEnvp()
{
	std::vector<std::string> env;

	// ===== Méthode / protocole =====
	env.push_back("REQUEST_METHOD=GET");
	env.push_back("SERVER_PROTOCOL=" + this->m_version);
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/1.0");

	// ===== Query string =====
	env.push_back("QUERY_STRING=" + this->m_query);

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

std::string	GetRequest::generateCGIResponse()
{
	//! debug
	std::cout << " generateCGIResponse() ->check" << std::endl;
	char **env = this->generateEnvp();
	std::cout << "env { \n" << std::endl;
	for (int i = 0; env[i] != NULL; i++)
	{
		std::cout << env[i] << std::endl;
	}
	std::cout << "\n } \n" << std::endl;

	std::string	cgi_response;
	int pipe_to_cgi[2];
    int pipe_from_cgi[2];
    pid_t pid = 0;

    if (pipe(pipe_to_cgi) == -1 || pipe(pipe_from_cgi) == -1) {
        //? return une erreur
    }
    pid = fork();
    if  (pid == -1) {
        close(pipe_to_cgi[0]);
        close(pipe_to_cgi[1]);
        close(pipe_from_cgi[0]);
        close(pipe_from_cgi[1]);
        //? return une erreur
    }
    else if (pid == 0) { //! child -> oubie qu'il est un serveur et execute le script
        //? ecrit dans [1](write) et lis dans [0](read)
        //! double tableau pour execve + recuperation de l'env
        //! ecrire dans file_from_cgi[1]
        //? dup stdin dans l'ecriture de pipe_from_cgi[1];

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
        args[0] = (char *) _bin.c_str()/*(char *) GetValue((const char *)"PATH_TRANSLATED", (const char **) env)*/;
        args[1] = (char *) _script.c_str()/*(char *) this->GetRealPath().c_str()*/;
        args[2] = NULL;
        if (execve(args[0], args, env) == -1) {
            //? return une erreur, le script ne sait pas executer + free
            perror("execve error");
            exit(1);
        }
    }
    //! parent ici -> erit dans la pipe_to_cgi[1]
    //!            -> lis dans pipe_from_cgi[0]
    close(pipe_to_cgi[0]);
    close(pipe_from_cgi[1]);
    close(pipe_to_cgi[1]);
    char buffer[4096];
    ssize_t bits_read;
    while ((bits_read = read(pipe_from_cgi[0], buffer, sizeof(buffer))) > 0) {
        cgi_response.append(buffer, bits_read);
    }
    if (bits_read == -1) {
        perror("read");
        //? continuer le reste ou faire un throw
    }
    close(pipe_from_cgi[0]);
    waitpid(pid, NULL, 0);
    return (cgi_response);
}
