#include "Location_config.hpp"

Location_config::Location_config(){    
}

/*-----------------------SETTER----------------------------------*/

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

void Location_config::setClientMaxBodySize(size_t size)
{
    _clientMaxBodySize = size;
}







/*---------------------------UTILS--------------------------------*/
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
}