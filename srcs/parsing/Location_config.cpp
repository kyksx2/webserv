#include "parsing/Location_config.hpp"
#include "parsing/Server_config.hpp"

Location_config::Location_config() :
    _autoindex(false), _clientMaxBodySize(0)
{
    addAllowedMethod("GET");
    addAllowedMethod("POST");
}

Location_config::Location_config(const std::string& path)
    :  _path(path), _autoindex(0), _clientMaxBodySize(0)
{}

/*-----------------------SETTER----------------------------------*/

void Location_config::setPath(const std::string& path)
{
    if (!path.empty())
        _path = path;
}

void Location_config::setRoot(const std::string& root)
{
    if (!root.empty())
        _root = root;
}

void Location_config::addIndex(const std::string& index)
{
    if (!index.empty())
        _index.push_back(index);
}

void Location_config::addAllowedMethod(const std::string& method)
{
    if (!method.empty())
        _allowedMethods.push_back(method);
}

void Location_config::setAutoindex(bool value)
{
    _autoindex = value;
}

void Location_config::setUploadStore(const std::string& path)
{
    if (!path.empty())
        _uploadStore = path;
}

void Location_config::setCgi(const std::string& extension, const std::string& path)
{
    if (!extension.empty() && !path.empty())
        _cgiHandlers[extension] = path;
}

void Location_config::setRedirect(int code, const std::string& url)
{
    if (code == 0 || url.empty()) {
        // Est-ce un nombre ou une URL ?
        if (code > 99 && code < 600) {
            _redirect = std::make_pair(code , "");
        } else {
            _redirect = std::make_pair(302, url);
        }
    } 
    else {
        _redirect = std::make_pair(code, url);
    }
}

void Location_config::setErrorPage(int code, const std::string& path)
{
    if (code >= 300 && code < 600)
        _errorPages[code] = path;
}
//! --------------------------------------------------------------------------
void Location_config::setErrorPages(const std::map<int, std::string>& pages) {
    this->_errorPages = pages;
}

void Location_config::setIndex(const std::vector<std::string>& new_index) {
    this->_index = new_index;
}

void Location_config::setAllowedMetode(const std::string& new_metodes) {
    if (!new_metodes.empty())
        this->_allowedMethods.push_back(new_metodes);
}
//! --------------------------------------------------------------------------

void Location_config::setClientMaxBodySize(size_t size)
{
    if (size < SIZE_MAX)
        _clientMaxBodySize = size;
}

/*---------------------------GUETTER------------------------------*/

const std::string& Location_config::getPath() const
{
    return (_path);
}

const std::string& Location_config::getRoot() const
{
        return _root;
}

const std::vector<std::string>& Location_config::getIndex() const
{

    return (_index);
}

const std::vector<std::string>& Location_config::getAllowedMethods() const
{
    return (_allowedMethods);
}

bool Location_config::isAutoindexEnabled() const
{
    return (_autoindex);
}

const std::string& Location_config::getUploadStore() const
{
    return (_uploadStore);
}

const std::map<std::string, std::string>& Location_config::getCgiHandlers() const
{
    return this->_cgiHandlers;
}

const std::pair<int, std::string>& Location_config::getRedirect() const
{
    return (_redirect);
}

const std::map<int, std::string>& Location_config::getErrorPages() const
{
    return (_errorPages);
}

size_t Location_config::getClientMaxBodySize() const
{
    return (_clientMaxBodySize);
}


/*---------------------------UTILS--------------------------------*/

bool Location_config::isMethodAllowed(const std::string& method) const
{
    for (size_t i = 0; i < _allowedMethods.size(); ++i)
    {
        if (_allowedMethods[i] == method)
            return (true);
    }
    return (false);
}

bool Location_config::isCgiRequest(const std::string& filename) const
{
    size_t pos = filename.find_last_of(".");

    if (pos == std::string::npos)
        return (false);
    std::string extension = filename.substr(pos);
    if (_cgiHandlers.count(extension) > 0)  
        return (true);
    return (false);
}

void Location_config::print() const {
    std::cout << "    Location: " << _path << std::endl;
    if (!_root.empty())
        std::cout << "      root: " << _root << std::endl;
    if (!_index.empty()) {
        std::cout << "      index: ";
        for (size_t i = 0; i < _index.size(); i++) {
            std::cout << _index[i];
            if (i < _index.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    if (!_allowedMethods.empty()) {
        std::cout << "      allowed_methods: ";
        for (size_t i = 0; i < _allowedMethods.size(); i++) {
            std::cout << _allowedMethods[i];
            if (i < _allowedMethods.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    if (_autoindex)
        std::cout << "      autoindex: on" << std::endl;
    if (!_uploadStore.empty())
        std::cout << "      upload_store: " << _uploadStore << std::endl;
    if (!_cgiHandlers.empty())
    {
        std::cout << "      cgi_handlers: " << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = _cgiHandlers.begin(); 
             it != _cgiHandlers.end(); 
             ++it)
            std::cout << "        " << it->first << " -> " << it->second << std::endl;
    }
    if (_redirect.first != 0)
        std::cout << "      redirect: " << _redirect.first << " " << _redirect.second << std::endl;
    if (_clientMaxBodySize != 0)
        std::cout << "      client_max_body_size: " << _clientMaxBodySize << std::endl;
    if (!_errorPages.empty()) {
        std::cout << "    error_pages:" << std::endl;
        for (std::map<int, std::string>::const_iterator it = _errorPages.begin();
             it != _errorPages.end(); ++it) {
            std::cout << "      " << it->first << " -> " << it->second << std::endl;
        }
    }
}
