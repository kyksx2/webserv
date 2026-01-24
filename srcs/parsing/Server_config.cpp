#include "parsing/Server_config.hpp"
#include "parsing/parsing.hpp"
#include "parsing/Global_Config.hpp"

Server_Config::Server_Config() :
    _host("0.0.0.0"),
    _port(8080),
    _root("html"),
    _autoindex(false),
    _clientMaxBodySize(1048576)
{
    _index.push_back("index.html");
}

Server_Config::~Server_Config(){
}

/* ----------------------- Setters ------------------------------------------*/

void Server_Config::setHost(const std::string& host)
{
    if (!host.empty())
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

void Server_Config::setRoot(const std::string& root)
{
    if (!root.empty())
        _root = root;
}

void Server_Config::addIndex(const std::string& index)
{
    if (!index.empty())
        _index.push_back(index);
}

void Server_Config::setAutoindex(bool value)
{
    _autoindex = value;
}

void Server_Config::setCgi(const std::string& extension, const std::string& path)
{
    if (!extension.empty() && !path.empty())
        _cgiHandlers[extension] = path;
}

void Server_Config::setErrorPage(int code, const std::string& path)
{
    if (code >= 300 && code < 600)
        _errorPages[code] = path;
}

void Server_Config::setClientMaxBodySize(size_t size)
{
    if (size < SIZE_MAX)
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

const std::string& Server_Config::getRoot() const
{
    return (_root);
}

const std::vector<std::string>& Server_Config::getIndex() const
{
    return (_index);
}

bool Server_Config::isAutoindexEnabled() const
{
    return (_autoindex);
}

const std::map<std::string, std::string>& Server_Config::getCgiHandlers() const
{
    return this->_cgiHandlers;
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

const Location_config* Server_Config::findLocation(const std::string& uri) const
{
    const Location_config *locationFind = NULL;
    size_t best_size = 0;

    for (size_t i = 0; i < _locations.size(); ++i)
    {
        const std::string &path = _locations[i].getPath();
        if (uri.compare(0, path.length(), path) == 0)
        {
            if (best_size < path.length())
            {
                locationFind = &_locations[i];
                best_size = path.length();
            }
        } 
    }
    return (locationFind);
}

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
    if (!_root.empty())
        std::cout << "    root: " << _root << std::endl;
    if (!_index.empty()) {
        std::cout << "    index: ";
        for (size_t i = 0; i < _index.size(); i++) {
            std::cout << _index[i];
            if (i < _index.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }

    if (_autoindex)
        std::cout << "      autoindex: on" << std::endl;

    if (_clientMaxBodySize != 0)
        std::cout << "    client_max_body_size: " << _clientMaxBodySize << std::endl;
    if (!_cgiHandlers.empty())
    {
        std::cout << "    cgi_handlers: " << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = _cgiHandlers.begin(); 
             it != _cgiHandlers.end(); 
             ++it)
            std::cout << "      " << it->first << " -> " << it->second << std::endl;
    }
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

void Server_Config::cleanIndex()
{
    if (!_index.empty())
        _index.clear();
}
//! -------------------------------------------------------------------
const Location_config& Server_Config::generateDefaultLocation() const {
    this->_defaultLocation->setPath("/");
    this->_defaultLocation->setRoot(this->getRoot());
    this->_defaultLocation->setIndex(this->_index);
    this->_defaultLocation->setAllowedMetode("GET");
    this->_defaultLocation->setAutoindex(false);
    this->_defaultLocation->setClientMaxBodySize(this->getClientMaxBodySize());
    this->_defaultLocation->setUploadStore("");
    this->_defaultLocation->setRedirect(0, "");
    this->_defaultLocation->setErrorPages(this->getErrorPages());
    std::cout << "----- VERIF DEFAULT LOCATION -----" << std::endl;
    this->_defaultLocation->print();
}
//! -------------------------------------------------------------------