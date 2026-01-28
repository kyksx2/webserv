/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:45:13 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/28 18:24:53 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/GetRequest.hpp"
#include "CGI/CgiHandler.hpp"

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
	std::string		realPath = this->m_target;
	struct stat st;

	//! faire le menage une fois tt pret
	std::cout << "envp = \n" << std::endl;
	for (int i = 0; this->generateEnvp()[i] != NULL; i++)
	{
		std::cout << this->generateEnvp()[i] << std::endl;
	}
	// Verifie si la Methode est autorise sur target
	const Location_config* location = this->m_serv.sendALocation(realPath);
	if (location) // theoriquement jms NULL | possibilité de changer pointeur en ref
	{
		realPath = location->getRoot() + this->m_target;
		if (location->isMethodAllowed("GET") == false)
		{
			getresponse.setStatus(405, "Method Not Allowed");
			location->getAllowedMethods();
			std::string method_allowed = vstos(location->getAllowedMethods(), ", ");
			getresponse.setHeader("Allow", method_allowed);
			return getresponse;
		}
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
				if (location->isAutoindexEnabled())
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
