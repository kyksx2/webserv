#include "Server_config.hpp"
#include "parsing.hpp"
#include "Config.hpp"


Server_Config::Server_Config(){

}

Server_Config::~Server_Config(){
}

/* ----------------------- Setters ------------------------------------------*/

void Server_Config::setHost(const std::string& host)
{
    if (host.find(':') == 4)
        _host = host;
}
void Server_Config::setPort(int port)
{
    if (port >= 1 && port <= 65535)
        _port = port;
}

void Server_Config::addServerName(const std::string& name)
{
    _serverNames.push_back(name);
}

void Server_Config::setErrorPage(int code, const std::string& path)
{
    if (code >= 300 && code < 600)
        _errorPages[code] = path;
}

void Server_Config::setClientMaxBodySize(size_t size)
{
    if (size < 2000000)
        _clientMaxBodySize = size;
}

void Server_Config::addLocation(const Location_config& location)
{
    _locations.push_back(location);
}

/*-------------------------- Getters ----------------------------------------------------*/

const std::string& Server_Config::getHost() const
{
    return (_host);
}

int Server_Config::getPort() const
{
    return (_port);
}

const std::vector<std::string>& Server_Config::getServerNames() const
{
    return (_serverNames);
}

const std::map<int, std::string>& Server_Config::getErrorPages() const
{
    return (_errorPages);
}

size_t Server_Config::getClientMaxBodySize() const
{
    return (_clientMaxBodySize);
}

const std::vector<Location_config>& Server_Config::getLocations() const
{
    return (_locations);
}
