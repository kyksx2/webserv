#ifndef CGIHANDLER_H
#define CGIHANDLER_H
#include "../serv_handler/Webserv.hpp"

class CgiHandler {
public:
	CgiHandler(char **env, std::string body);
	CgiHandler(const CgiHandler& src);
	~CgiHandler();

	CgiHandler& operator=(const CgiHandler& src);

	void executeCgi();
private:
	std::string _binPath;
	std::string _scriptPath;
	std::string _response_cgi;
	std::string _body;
	char** _env;
};

#endif