#ifndef CLIENT_H
#define CLIENT_H
#include "Server.hpp"
#include "request_response/HTTPResponse.hpp"
#include "request_response/HTTPRequest.hpp"
#include "request_response/RequestMethod.hpp"

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

private:
	int				client_fd;
	Server*			dad_serv;
	time_t			start;
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

// class Client {
//  public:
// 	Client();
// 	Client(int fd, Server* find_server);
// 	~Client();

// 	void appendRequest(const char* request, int size);
// 	bool completeRequest();
// 	void parseRequest();
// 	void generateResponse();
// 	void requestCreation();
// 	bool isKeepAlive();
// 	void clearState();
// 	void restartTimer();

// 	std::string& getResponseBuffer();
// 	int getClientFd();
// 	size_t getDataSent();
// 	std::string& getRequestBuffer();
// 	time_t	getStart();


//  private:
// 	time_t start; //? new a ajouter
// 	int client_fd;
// 	size_t data_sent;
// 	Server* dad_serv;
// 	bool	hasresponse;
// 	bool headerParse;
// 	size_t headerSize;
// 	size_t contentLength;
// 	bool isChunked;
// 	std::string requestBuffer;
// 	std::string responseBuffer;
// 	HTTPRequest *request;
// 	HTTPResponse response;
// };

// class Client {
//  public:
// 	Client(int fd, Server* find_server);
// 	~Client();
// 	Client(const Client& src);
// 	Client&  operator=(const Client& src);

// 	void appendRequest(const char* request, int size);
// 	bool completeRequest();
// 	//void parseRequest();
// 	//void generateResponse();
// 	void requestCreation();
// 	bool isKeepAlive();
// 	void clearState();

// 	std::string& getResponseBuffer();
// 	int getClientFd();
// 	size_t getDataSent();
// 	std::string& getRequestBuffer();

// 	void setDataSent(int n);
// 	void setResponseBuffer(std::string& response);

//  private:
// 	int client_fd;
// 	size_t data_sent;
// 	Server* dad_serv;
// 	bool headerParse;
// 	size_t contentLength;
// 	bool isChunked;
// 	std::string requestBuffer;
// 	std::string responseBuffer;
// 	HTTPRequest *request;
// 	HTTPResponse response;
// };
#endif