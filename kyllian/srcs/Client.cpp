#include "../include/Client.hpp"
#include "../include/RequestMethod.hpp"

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

Client::Client(int fd, Server* find_server) : client_fd(fd), data_sent(0), dad_serv(find_server), requestBuffer(""), responseBuffer("") {}

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

size_t  Client::getDataSent() { return this->data_sent; }

int Client::getClientFd() { return this->client_fd; }

void    Client::setDataSent(int n) { this->data_sent = n; }


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

bool    Client::completeRequest()
{
	try
    {
        requestCreation();
		return true;
    }
    catch(const std::exception& e)
    {
		std::string	error = e.what()
		if (error == "restart getbuffer")
		{
			// liberez tout mes copain nan en sah delete request si il faut
			return false;
		}
        //! il faut verifier si response existe deja
        std::vector<std::string> err_line = split(e.what(), ',');
        // ajout de std::atoi
        HTTPResponse response(err_line[0], std::atoi(err_line[1].c_str()), err_line[2]);
		return true;
    }
}

void    Client::generateResponse() {
    try
    {
        requestCreation();
    }
    catch(const std::exception& e)
    {
        //! il faut verifier si response existe deja
        std::vector<std::string> err_line = split(e.what(), ',');
        // ajout de std::atoi
        HTTPResponse response(err_line[0], std::atoi(err_line[1].c_str()), err_line[2]);
    }
}

//	Utilise le polymorphisme pour creer la bonne classe
void Client::requestCreation()
{
	//! pensez a delete request && a verifier si response existe avent d essayer de le creer grace a generate (= solution envisageable throw)
    HTTPRequest     *request;
    HTTPResponse    response;
    int    i;
    std::string method[] = {"GET", "POST", "DELETE"};
	const	Server &serv = *(this->dad_serv); // conversion pointeur -> reference
    HTTPRequest *(*ft_method[])(std::string, const Server&) = {
        get_creation,
        delete_creation,
        post_creation
    };
    std::stringstream ss(this->requestBuffer);
    std::string line;
    std::getline(ss, line);
    std::vector<std::string> firstline = split(line, ' ');
    if (firstline.size() < 3)
        this->response = HTTPResponse ("HTTP/1.1", 400, "Bad Request");

    std::string    method_buffer = firstline[0];
    std::string    version = firstline[2];

    try
    {
        for (i = 0; i < 3; i++)
        {
			//! request peut etre vide attention au segfault lors de la creation de .env
            if (method[i] == method_buffer)
            { // plus besoin de creer response direct
                request = ft_method[i](this->requestBuffer, serv);
            }
        }

        if (version == "HTTP/1.0" || version == "HTTP/1.1")
            this->response = HTTPResponse (version , 400, "Bad Request");

            throw std::runtime_error("400 Bad Request");
    }
    catch (const std::exception& e)
    {
        this->response = HTTPResponse("HTTP/1.1", 400, "Bad Request");
    }
    this->response = response;
}