#ifndef CLIENT_H
#define CLIENT_H
#include "Server.hpp"
#include "request_response/HTTPResponse.hpp"
#include "request_response/HTTPRequest.hpp"
#include "request_response/RequestMethod.hpp"

class HTTPRequest;

class Client {
 public:
	Client();
	Client(int fd, Server *find_server);
	Client(const Client& src);
	Client&	operator=(const Client& src);
	~Client();

	void	appendRequest(const char* request, int size);
	bool	completeRequest();
	void	requestCreation();
	void	generateBufferResponse();
	void clearState();
	void	restartTimer();
	int	getClientFd() const;
	Server*	getServer() const;
	std::string&	getRequestBuffer();
	std::string&	getResponseBuffer();
	size_t	getDataSent() const;
	bool	isKeepAlive();
	time_t	getStart() const;
	bool	hasResponse() const;
	void	setDataSent(size_t n);
	void	setResponseBuffer(std::string& response);
	time_t			getStart();

	void setRequest(std::string &buffer);
	void printHeader();
	void printBufferResponse();


private:
	int	client_fd;
	Server*	dad_serv;
	time_t	start;
	size_t	data_sent;
	bool	headerParse;
	size_t	headerSize;
	size_t	contentLength;
	bool	isChunked;
	bool	parsresponse;
	std::string	requestBuffer;
	std::string	responseBuffer;
	HTTPRequest* request;
	HTTPResponse response;
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