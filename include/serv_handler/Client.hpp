#ifndef CLIENT_H
#define CLIENT_H
#include "Server.hpp"
#include "request_response/HTTPResponse.hpp"

class HTTPRequest;

class Client {
 public:
	Client(int fd, Server *find_server);
	Client(const Client& src);
	Client&	operator=(const Client& src);
	~Client();

	void	appendRequest(const char* request, int size);
	bool	completeRequest();
	void	requestCreation();
	void    CreateResponse(std::string version, int status_code, std::string reason_phrase);
	void	generateBufferResponse(int epoll_fd, std::map<int, Client*>& client_map, Client* client);
	void	clearState();
	void	restartTimer();
	bool	isKeepAlive();
	bool	isCGI(const HTTPRequest* req);
	void	printHeader();
	void	printBufferResponse();
	void	appendRequestCgi(const char* buffer, int n_size);
	void	completeCgi();

	time_t		getStart() const;
	time_t		getstartCgi() const;
	int			getClientFd() const;
	std::string	getRequestBuffer() const;
	std::string	getResponseBuffer() const;
	size_t		getDataSent() const;
	bool		getActiveCgi() const;
	pid_t		getCgiPid() const;
	int			getCgiFd() const;
	std::string	getCgiBuffer() const;

	void	setDataSent(size_t n);
	void	setResponseBuffer(std::string& response);
	void	setRequest(std::string &buffer);
	void	setCgiStatus(bool status);
	void	setCgiFd(int fd);
	void	setCgiPid(pid_t pid);
	void	setStartCgi(time_t t);

private:
	int				client_fd;
	Server*			dad_serv;
	time_t			start;
	time_t			start_cgi;
	size_t			data_sent;
	bool			headerParse;
	size_t			headerSize;
	size_t			contentLength;
	bool			isChunked;
	bool			hasresponse;
	std::string		requestBuffer;
	std::string		responseBuffer;
	HTTPRequest*	request;
	HTTPResponse	response;
	bool			active_cgi;
	int				cgi_fd;
	pid_t			cgi_pid;
	std::string		cgiBuffer;
};
#endif