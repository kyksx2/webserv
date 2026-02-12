#include "serv_handler/Client.hpp"
#include "request_response/PostRequest.hpp"
#include "request_response/GetRequest.hpp"
#include "request_response/DeleteRequest.hpp"

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

Client::Client(int fd, Server* find_server) : client_fd(fd), dad_serv(find_server), start_cgi(0), data_sent(0), headerParse(false), headerSize(0),contentLength(0),
    isChunked(false), hasresponse(false), requestBuffer(""), responseBuffer(""), request(), response(), active_cgi(false), cgi_fd(-1), cgi_pid(0) {
        start = time(NULL);
}

Client::Client(const Client& src) {
    *this = src;
}

Client&  Client::operator=(const Client& src) {
    if (this != &src) {
        this->client_fd = src.client_fd;
		this->contentLength = src.contentLength;
		this->dad_serv = src.dad_serv;
		this->start_cgi = src.start_cgi;
		this->data_sent = src.data_sent;
		this->headerParse = src.headerParse;
		this->headerSize = src.headerSize;
		this->isChunked = src.isChunked;
		this->hasresponse = src.hasresponse;
		this->request = src.request;
		this->requestBuffer = src.requestBuffer;
		this->response = src.response;
		this->responseBuffer = src.responseBuffer;
		this->start = src.start;
		this->active_cgi = src.active_cgi;
		this->cgi_fd = src.cgi_fd;
		this->cgi_pid = src.cgi_pid;
    }
    return (*this);
}

Client::~Client() {
	if (this->request)
		delete request;
}

size_t  Client::getDataSent() const { return this->data_sent; }

int Client::getClientFd() const { return this->client_fd; }

std::string Client::getRequestBuffer() const { return this->requestBuffer; }

std::string Client::getCgiBuffer() const { return this->cgiBuffer; }

std::string Client::getResponseBuffer() const { return this->responseBuffer; }

time_t Client::getStart() const { return (this->start); }

time_t Client::getstartCgi() const { return (this->start_cgi); }

pid_t	Client::getCgiPid() const { return this->cgi_pid; }

int	Client::getCgiFd() const { return this->cgi_fd; }

bool	Client::getActiveCgi() const { return this->active_cgi; }

void    Client::setDataSent(size_t n) { this->data_sent = n; }

void    Client::setResponseBuffer(std::string& response) { this->responseBuffer = response; }

// POST /dossier/page.html?query=123 HTTP/1.1\r\n      <-- 1. Request Line
// Host: localhost:8080\r\n                            <-- 2. Headers
// User-Agent: curl/7.68.0\r\n
// Content-Length: 15\r\n
// \r\n                                                <-- 3. Séparateur
// nom=bob&age=22                                      <-- 4. Body

void	Client::setRequest(std::string& buffer) { this->requestBuffer = buffer; }

void	Client::setCgiStatus(bool status) { this->active_cgi = status; }

void	Client::setCgiFd(int fd) { this->cgi_fd = fd; }

void	Client::setCgiPid(pid_t pid) { this->cgi_pid = pid; }

void	Client::setStartCgi(time_t t) { this->start_cgi = t; }


bool Client::isKeepAlive() { return this->response.IsKeepAlive(); }

void    Client::appendRequest(const char* request, int size) {
    if (!request || size <= 0)
        return;
    this->requestBuffer.append(request, size);
}

void    Client::appendRequestCgi(const char* buffer, int n_size) {
    if (!buffer || n_size <= 0)
        return;
    this->cgiBuffer.append(buffer, n_size);
}

void    Client::clearState() {
    this->data_sent = 0;
    this->responseBuffer.clear();
    this->requestBuffer.clear();
}

void Client::printHeader()
{
	std::cout << "---- HEADER -----" << std::endl;
	std::cout << this->requestBuffer.substr(0, this->headerSize - 4) << std::endl;
	std:: cout << " ---------------- " << std::endl;
	std::cout << *(this->request);
}

void Client::restartTimer() { this->start = time(NULL); } //?????????????????????????????????? changement


// POST /dossier/page.html?query=123 HTTP/1.1\r\n      <-- 1. Request Line
// Host: localhost:8080\r\n                            <-- 2. Headers
// User-Agent: curl/7.68.0\r\n
// Content-Length: 15\r\n
// \r\n                                                <-- 3. Séparateur
// nom=bob&age=22                                      <-- 4. Body

//	Utilise le polymorphisme pour creer la bonne classe
void Client::requestCreation()
{
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
		this->hasresponse = true;
		return;
	}

	std::string		method_buffer = firstline[0];
	std::string		version = firstline[2];

	try
	{
		for (i = 0; i < 3; i++)
		{
			if (method[i] == method_buffer)
			{ // plus besoin de creer response direct
				this->request = ft_method[i](this->requestBuffer, serv);
				return ;
			}
		}

		if (version == "HTTP/1.0" || version == "HTTP/1.1")
		{
			this->response = HTTPResponse (version , 400, "Bad Request");
			this->hasresponse = true;
			return;
		}
		throw std::runtime_error("400 Bad Request");
	}
	catch (const std::exception& e)
	{
		this->response = HTTPResponse("HTTP/1.1", 400, "Bad Request");
		this->hasresponse = true;
		return;
	}
}

// Creer la classe request lors de la premiere iteration, son body lors de la deuxieme
// ou la classe response (Bad Request) si probleme
bool Client::completeRequest()
{
	// ===== HEADER =====
	if (!this->headerParse)
	{
		size_t pos = this->requestBuffer.find("\r\n\r\n");
		if (pos == std::string::npos)
			return false;
		try
		{
			requestCreation();
			this->headerParse = true;

			if (this->hasresponse)
				return true;

			if (this->request->GetHeaders_value("transfer-encoding") == "chunked")
			{
				this->isChunked = true;
			}
			else
			{
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
		}
		catch (const std::exception& e)
		{
			std::vector<std::string> err = split(e.what(), ',');
			this->response = HTTPResponse(err[0], atoi(err[1].c_str()), err[2]);
			this->hasresponse = true;
			return true;
		}
	}
	if (this->isChunked)
	{
		size_t pos = this->requestBuffer.find("\r\n\r\n");
		if (pos == std::string::npos)
   			return false;
		std::string body = this->requestBuffer.substr(pos + 4);
		// size_t posb = body.find("0\r\n\r\n");
		// if (posb == std::string::npos)
		// 	return false;
		try
		{
			this->request->SetBody_Chunked(body); // parser chunk par chunk

			// Vérifier taille maximale APRÈS parsing
			if (dad_serv)
			{
				if (this->request->GetBody().size() > (size_t)this->dad_serv->getConfig().getClientMaxBodySize())
				{
					this->response = HTTPResponse(this->request->GetVersion(), 413, "Payload Too Large");
					this->hasresponse = true;
					return true;
				}
			}
			std::cout << *(this->request) << std::endl;
			return true; // body complet et valide
		}
		catch(const std::exception& e)
		{
			return false;
		}
	}
	else
	{
			// ===== BODY -Content-Lenght- =====
		size_t pos = this->requestBuffer.find("\r\n\r\n");
		std::string body = this->requestBuffer.substr(pos + 4);

		if (body.size() < this->contentLength)
			return false; // attendre plus de données
		try
		{
			this->request->SetBody_ContentLength(body);
			return true;
		}
		catch (const std::exception& e)
		{
			std::vector<std::string> err = split(e.what(), ',');
			this->response = HTTPResponse(err[0], atoi(err[1].c_str()), err[2]);
			this->hasresponse = true;
			return true;
		}
	}
	return true;
}

// attribue en generant si besoin le buffer response a l attribut responseBuffer
// reinitialise au passage les attributs de verification
void	Client::generateBufferResponse(int epoll_fd, std::map<int, Client*>& client_map, Client* client)
{
	this->responseBuffer.clear();
	if (this->hasresponse)
		this->responseBuffer = this->response.generate();
	else if (this->request)
	{
		if (isCGI(this->request))
			this->request->startCgi(epoll_fd, client_map, client);
		else
		{

			std::cout << this->request->GetBody() << std::endl;
			this->response = this->request->generateResponse();
			this->responseBuffer = this->response.generate();
		}

	}
	//! penser a remettre tout a 0
	this->headerParse = false;
	this->headerSize = 0;
	this->contentLength = 0;
	this->isChunked = false;
	this->hasresponse = false;
	if (this->request)
	{
		delete this->request;
		this->request = NULL;
	}

}

void Client::printBufferResponse()
{
	std::cout << this->responseBuffer;
}

bool Client::isCGI(const HTTPRequest *req)
{
	const Location_config *loc = req->Getlocation();
	std::string path = req->GetTarget();
	std::string ext = req->GetExtension();

	if (!loc->isCgiRequest(req->GetRealPath()))
		return false;

	std::map<std::string, std::string> cgiHandlers = loc->getCgiHandlers();
	if (cgiHandlers.count(ext))
		return true;

	return false;
}

void Client::completeCgi() {
	this->data_sent = 0;

	if (this->hasresponse)
	{
		this->responseBuffer = this->response.generate();
	}
	else if (this->request) {
		if (this->isCGI(request))
		{
			this->response = HTTPResponse(this->request->GetVersion(), this->cgiBuffer);
			this->responseBuffer = this->response.generate();
		}
		delete this->request;
		this->request = NULL;
	}
	this->cgiBuffer.clear();
	this->active_cgi = false;
	this->headerParse = false;
	this->requestBuffer.clear();
	this->headerSize = 0;
	this->contentLength = 0;
	this->isChunked = false;
	this->hasresponse = false;

}