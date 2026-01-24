/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:45:13 by yzeghari          #+#    #+#             */
/*   Updated: 2026/01/24 15:15:37 by kjolly           ###   ########.fr       */
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

//Vector to std::string Conversion
std::string vstos(const std::vector<std::string>& v, const std::string& separateur)
{
	std::string str;

	for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
	{
		if (it != v.begin())
			str += separateur;
		str += *it;
	}
	return str;
}

HTTPResponse GetRequest::generateResponse()
{
	//tester path../file/ != path../dir/
	std::string		realPath = this->m_target;
	HTTPResponse	getresponse;
	getresponse.setVersion(this->m_version);
	getresponse.setHeader("connection", this->m_headers["connection"]);
	std::string	inthefile;
	struct stat st;

	//! faire le menage une fois tt pret
	// Verifie si la Methode est autorise sur target
	const Location_config* location = this->m_serv.sendALocation(realPath);
	if (location == NULL)
	{
		//! let see if you realy trap
		//protection a rajouter plus tard
		std::cout << "Besoin de la partie demande a kylian";
	}
	else
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

	std::cout << realPath << std::endl;
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

