#include "../include/Server_config.hpp"
#include "../include/parsing.hpp"
#include "../include/Config.hpp"


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

/*-----------------------UTILS-----------------------------*/

void Server_Config::print() const {
    std::cout << "  Server:" << std::endl;
    std::cout << "    listen: " << _host << ":" << _port << std::endl;
    
    if (!_serverNames.empty()) {
        std::cout << "    server_name: ";
        for (size_t i = 0; i < _serverNames.size(); i++) {
            std::cout << _serverNames[i];
            if (i < _serverNames.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    
    if (_clientMaxBodySize != 0)
        std::cout << "    client_max_body_size: " << _clientMaxBodySize << std::endl;
    
    if (!_errorPages.empty()) {
        std::cout << "    error_pages:" << std::endl;
        for (std::map<int, std::string>::const_iterator it = _errorPages.begin();
             it != _errorPages.end(); ++it) {
            std::cout << "      " << it->first << " -> " << it->second << std::endl;
        }
    }
    
    std::cout << "    Locations (" << _locations.size() << "):" << std::endl;
    for (size_t i = 0; i < _locations.size(); i++) {
        _locations[i].print();
    }
}