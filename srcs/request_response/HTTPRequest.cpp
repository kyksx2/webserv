/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:45:23 by yzeghari          #+#    #+#             */
/*   Updated: 2026/02/02 16:29:52 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response/HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::string &buffer, const Server& serv)
{
	std::stringstream ss(buffer);
	std::string line;
	std::getline(ss, line);

	this->m_serv = serv;
	std::vector<std::string> firstline = split(line, ' ');
	if (firstline.size() < 3)
		throw HTTPRequest::HTTPRequestException("HTTP/1.1,400,Bad Request");

	std::string method = firstline[0];
	std::string target = firstline[1];
	std::string version = firstline[2];

	// nettoyage du \r final sur la version
	if (!version.empty() && version[version.length() - 1] == '\r')
		version.erase(version.length() - 1);

	m_version = version;
	if (m_version != "HTTP/1.0" && m_version != "HTTP/1.1")
		throw HTTPRequest::HTTPRequestException("HTTP/1.1,501,Not Implemented");

	// validation méthode minimale
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw HTTPRequest::HTTPRequestException(m_version + ",405,Method Not Allowed");

	// séparateur query
	size_t qpos = target.find('?');
	if (qpos != std::string::npos)
	{
		m_target = target.substr(0, qpos);
		m_query = target.substr(qpos + 1);
	}
	else
		m_target = target;

	if (target.empty() || target[0] != '/')
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

	while (std::getline(ss, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		if (line.empty())
			break;

		// séparateur clé/valeur
		size_t pos = line.find(':');
		if (pos == std::string::npos)
			throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

		std::string key = line.substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		std::string value = line.substr(pos + 1);

		// trim espaces au début de la valeur
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		// headers multiples
		if (m_headers.count(key))
			m_headers[key] += ", " + value;
		else
			m_headers[key] = value;
	}

	// Transfer-Encoding + HTTP/1.0
	if (m_headers.count("transfer-encoding") && m_headers.count("content-length"))
	{
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
	}

	// Transfer-Encoding + Content-Length interdits ensemble
	if (m_headers.count("transfer-encoding") && m_headers.count("content-length"))
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

	// Connection par défaut (corrigé)
	if (!m_headers.count("connection"))
	{
		if (m_version == "HTTP/1.1")
			m_headers["connection"] = "keep-alive";
		else
			m_headers["connection"] = "close";
	}
	else
	{
		if (m_headers["connection"] != "keep-alive" &&
			m_headers["connection"] != "close")
			throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");
	}
	// Host obligatoire en HTTP/1.1
	if (!m_headers.count("host") && m_version == "HTTP/1.1")
		throw HTTPRequest::HTTPRequestException(m_version + ",400,Bad Request");

	// Location
	this->m_location = this->m_serv.sendALocation(this->m_target);
	if (this->m_location == NULL)
	{
		// theoriquement impossible | ce message est la pour debug au cas ou qd meme
		std::cerr << "location == NULL peut causer un segfault" << std::endl;
	}
}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &src)
{
	if (this == &src)
		return (*this);
	this->m_serv = src.m_serv;
	this->m_location = src.m_location;
	this->m_target = src.m_target;
	this->m_query = src.m_query;
	this->m_version = src.m_version;
	this->m_headers = src.m_headers;
	this->m_body = src.m_body;
	return *this;
}

HTTPRequest::~HTTPRequest()
{
}

void HTTPRequest::SetBody(std::string &buffer)
{
	int contentLength = 0;
	if (!safe_atoi(m_headers["content-length"].c_str(), contentLength) || contentLength < 0)
		throw HTTPRequestException(m_version + ",400,Bad Request");

	if (contentLength > (int) this->m_serv.getConfig().getClientMaxBodySize())
		throw HTTPRequestException(m_version + ",413,Payload Too Large");

	m_body = buffer.substr(0, contentLength);
}

const Location_config *HTTPRequest::Getlocation() const
{
	return this->m_location;
}

std::string HTTPRequest::GetTarget() const
{
	return (this->m_target);
}

std::string HTTPRequest::GetRealPath() const
{
	std::string	realPath = this->m_location->getRoot() + this->m_target;
	return (realPath);
}

std::string HTTPRequest::GetExtension() const
{
    std::size_t pos = m_target.find_last_of('.');
    if (pos == std::string::npos || pos == m_target.length() - 1)
        return "";

    return m_target.substr(pos);
}


std::string HTTPRequest::GetQuery() const
{
	return (this->m_query);
}


std::string HTTPRequest::GetVersion() const
{
	return (this->m_version);
}

std::string HTTPRequest::GetBody() const
{
	return (this->m_body);
}

std::map<std::string, std::string> HTTPRequest::GetHeaders() const
{
	return (this->m_headers);
}

std::string HTTPRequest::GetHeaders_value(std::string key)
{
	std::string	val;

	if (this->m_headers.count(key))
	{
		val = this->m_headers[key];
	}
	return val;
}

std::string HTTPRequest::generateCGIResponse()
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

HTTPRequest::HTTPRequestException::HTTPRequestException(std::string err) throw()
{
	this->_err = err;
}

const char *HTTPRequest::HTTPRequestException::what() const throw()
{
	return (this->_err.c_str());
}

HTTPRequest::HTTPRequestException::~HTTPRequestException() throw()
{
}

const char *GetValue(const char *key, const char **env)
{
	int i = 0;
	size_t key_len = strlen(key);

	while (env && env[i])
	{
		if (strncmp(key, env[i], key_len) == 0
			&& env[i][key_len] == '=')
		{
			return (env[i] + key_len + 1);
		}
		i++;
	}
	return NULL;
}

std::ostream &operator<<(std::ostream &os, const HTTPRequest &req)
{
	os << "===== HTTPRequest =====" << std::endl;

	os << "Target  : " << req.GetTarget() << std::endl;
	os << "Version : " << req.GetVersion() << std::endl;

	os << "--- Query ---" << std::endl;
	os << "Query : " << req.GetQuery() << std::endl;
	os << "--- Headers ---" << std::endl;
	std::map<std::string, std::string> headers = req.GetHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		it != headers.end(); ++it)
	{
		os << it->first << ": " << it->second << std::endl;
	}

	os << "--- Body ---" << std::endl;
	os << req.GetBody() << std::endl;

	os << "=======================" << std::endl;
	return os;
}


std::vector<std::string>	split(const std::string &chaine, char delimiteur)
{
	std::vector<std::string>	v;
	std::stringstream ss(chaine);
	std::string	sous_chaine;

	while(std::getline(ss, sous_chaine, delimiteur))
	{
		v.push_back(sous_chaine);
	}
	return (v);
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
bool safe_atoi(const char *str, int &result)
{
	int sign = 1;
	int value = 0;

	if (!str)
		return false;

	while ((*str >= 9 && *str <= 13) || *str == ' ')
		str++;

	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}

	if (*str < '0' || *str > '9')
		return false;

	// Parse digits
	while (*str >= '0' && *str <= '9')
	{
		int digit = *str - '0';

		// Overflow check
		if (sign == 1)
		{
			if (value > (INT_MAX - digit) / 10)
				return false; // overflow
		}
		else
		{
			if (value > (-(INT_MIN + digit)) / 10)
				return false; // underflow
		}

		value = value * 10 + digit;
		str++;
	}

	result = value * sign;
	return true;
}
