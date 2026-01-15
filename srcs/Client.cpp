#include "../include/Client.hpp"

static HTTPRequest    *get_creation(std::string buffer, const Server &serv)
{
    return (new GetRequest(buffer, serv));
}

static HTTPRequest    *delete_creation(std::string buffer, const Server &serv)
{
    return (new DeleteRequest(buffer, serv));
}

static HTTPRequest    *post_creation(std::string buffer, const Server &serv)
{
    return (new PostRequest(buffer, serv));
}

Client::Client(int fd, Server* find_server) : client_fd(fd), dad_serv(find_server), data_sent(0),/*headerParse(false), headerSize(0),contentLength(0),
    isChunked(false), */requestBuffer(""), responseBuffer("") {
        start = time(NULL);
}

Client::Client(const Client& src) {
    *this = src;
}

Client&  Client::operator=(const Client& src) {
    if (this != &src) {
        this->client_fd = src.client_fd;
        this->data_sent = src.data_sent;
        this->dad_serv = src.dad_serv;
        // this->requestBuffer = src.requestBuffer;
        // this->responseBuffer = src.responseBuffer;
        //! pour les 2 classe request et responses verifier qu'il y a bien un constructeur de copie
    }
    return (*this);
}

Client::~Client() {}

bool Client::isKeepAlive() { return this->request->IsKeepAlive(); }

size_t  Client::getDataSent() const { return this->data_sent; }

int Client::getClientFd() const { return this->client_fd; }

std::string& Client::getRequestBuffer() { return this->requestBuffer; }

void    Client::setDataSent(size_t n) { this->data_sent = n; }


void    Client::appendRequest(const char* request, int size) {
    if (!request || size <= 0)
        return;
    this->requestBuffer.append(request, size);
}

void    Client::clearState() {
    this->data_sent = 0;
    this->responseBuffer.clear();
    this->requestBuffer.clear();
}

std::string& Client::getResponseBuffer() { return this->responseBuffer; }

void    Client::setResponseBuffer(std::string& response) {
    this->responseBuffer = response;
}

// POST /dossier/page.html?query=123 HTTP/1.1\r\n      <-- 1. Request Line
// Host: localhost:8080\r\n                            <-- 2. Headers
// User-Agent: curl/7.68.0\r\n
// Content-Length: 15\r\n
// \r\n                                                <-- 3. Séparateur
// nom=bob&age=22                                      <-- 4. Body

//! ici
Client::Client() : headerParse(false), headerSize(0), contentLength(0), isChunked(false), parsresponse(false), request(NULL) {}

// void	Client::setRequest(std::string& buffer) {
// 	this->requestBuffer = buffer;
// }

// void Client::printHeader() {
// 	std::cout << "---- HEADER -----" << std::endl;
// 	std::cout << this->requestBuffer.substr(0, this->headerSize - 4) << std::endl;
// 	std:: cout << " ---------------- " << std::endl;
// 	std::cout << *(this->request);
// }

void Client::restartTimer() { this->start = time(NULL); } //?????????????????????????????????? changement

time_t Client::getStart() { return (this->start); } //???????????????????????????????? changement

// void    Client::setResponseBuffer(std::string& response) {
//     this->responseBuffer = response;
// }

// POST /dossier/page.html?query=123 HTTP/1.1\r\n      <-- 1. Request Line
// Host: localhost:8080\r\n                            <-- 2. Headers
// User-Agent: curl/7.68.0\r\n
// Content-Length: 15\r\n
// \r\n                                                <-- 3. Séparateur
// nom=bob&age=22                                      <-- 4. Body

// void Client::printResponse() const
// {
// 	if (!this->parsresponse)
// 	{
// 		std::cout << "________________" << std::endl;
// 		std::cout << "Nothing to Print" << std::endl;
// 		std::cout << "________________" << std::endl;
// 		return ;
// 	}
// 	std::cout << this->response;
// }

//	Utilise le polymorphisme pour creer la bonne classe
void Client::requestCreation()
{
	//! pensez a delete request && a verifier si response existe avent d essayer de le creer grace a generate (= solution envisageable throw)
    // HTTPRequest     *request;
    HTTPResponse    response;
    int    i;
    std::string method[] = {"GET", "POST", "DELETE"};
	const	Server &serv = *(this->dad_serv); // conversion pointeur -> reference
    HTTPRequest *(*ft_method[])(std::string, const Server&) = {
        get_creation,
        post_creation,
        delete_creation
    };
    std::stringstream ss(this->requestBuffer);
    std::string line;
    std::getline(ss, line);
    std::vector<std::string> firstline = split(line, ' ');
	if (firstline.size() < 3)
	{
		this->response = HTTPResponse ("HTTP/1.1", 400, "Bad Request");
		this->parsresponse = true;
	}

	std::string		method_buffer = firstline[0];
	std::string		version = firstline[2];

	try
	{
		for (i = 0; i < 3; i++)
		{
			//! request peut etre vide attention au segfault lors de la creation de .env
			if (method[i] == method_buffer)
			{ // plus besoin de creer response direct
				this->request = ft_method[i](this->requestBuffer, serv);
				return ;
			}
		}

		if (version == "HTTP/1.0" || version == "HTTP/1.1")
		{
			this->response = HTTPResponse (version , 400, "Bad Request");
			this->parsresponse = true;
			return;
		}
		throw std::runtime_error("400 Bad Request");
	}
	catch (const std::exception& e)
	{
		this->response = HTTPResponse("HTTP/1.1", 400, "Bad Request");
		this->parsresponse = true;
		return;
	}
}

bool Client::completeRequest()
{
	size_t pos = this->requestBuffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return false;

	// ===== HEADER =====
	if (!this->headerParse)
	{
		try
		{
			this->isChunked = false;
			requestCreation();

			this->headerParse = true;

			if (this->parsresponse)
				return true;

			std::string cl = this->request->GetHeaders_value("content-length");
			if (cl.empty())
			{
				this->contentLength = 0;
				return true;
			}

			int tmp;
			if (!safe_atoi(cl.c_str(), tmp) || tmp < 0)
				throw HTTPRequest::HTTPRequestException("HTTP/1.1,400,Bad Request");

			this->contentLength = tmp;
		}
		catch (const std::exception& e)
		{
			std::vector<std::string> err = split(e.what(), ',');
			this->response = HTTPResponse(err[0], atoi(err[1].c_str()), err[2]);
			this->parsresponse = true;
			return true;
		}
	}

	// ===== BODY =====
	std::string body = this->requestBuffer.substr(pos + 4);

	if (body.size() < this->contentLength)
		return false; // attendre plus de données

	try
	{
		this->request->SetBody(body);
		return true;
	}
	catch (const std::exception& e)
	{
		std::vector<std::string> err = split(e.what(), ',');
		this->response = HTTPResponse(err[0], atoi(err[1].c_str()), err[2]);
		this->parsresponse = true;
		return true;
	}
}

void	Client::generateBufferResponse()
{
	this->responseBuffer.clear();

	if (this->parsresponse)
	{
		this->responseBuffer = this->response.generate();
	}
	else if (this->request)
	{
		this->response = this->request->generateResponse();
		this->responseBuffer = this->response.generate();
	}

	if (this->request)
	{
		delete this->request;
		this->request = NULL;
	}
}


// void Client::printBufferResponse()
// {
// 	std::cout << this->responseBuffer;
// }
