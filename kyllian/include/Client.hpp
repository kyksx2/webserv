#ifndef CLIENT_H
#define CLIENT_H
#include "Server.hpp"
#include "HTTPResponse.hpp"
#include "HTTPRequest.hpp"
#include "RequestMethod.hpp"

class HTTPRequest;

class Client {
 public:
	Client(int fd, Server* find_server);
	~Client();
	Client(const Client& src);
	Client&  operator=(const Client& src);

	void appendRequest(const char* request, int size);
	// bool completeRequest();
	//void parseRequest();
	//void generateResponse();
	void requestCreation();
	bool isKeepAlive();
	void clearState();
	void restartTimer();

	std::string& getResponseBuffer();
	int getClientFd();
	size_t getDataSent();
	std::string& getRequestBuffer();
	time_t	getStart();

	void setDataSent(int n);
	void setResponseBuffer(std::string& response);

 private:
	time_t start; //? new a ajouter
	int client_fd;
	size_t data_sent; 
	Server* dad_serv;
	// bool headerParse;
	// size_t headerSize;
	// size_t contentLength;
	// bool isChunked;
	std::string requestBuffer;
	std::string responseBuffer;
	HTTPRequest *request;
	HTTPResponse response;
};

#endif