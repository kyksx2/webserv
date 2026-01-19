#ifndef SERVEUR_CONFIG_HPP
#define SERVEUR_CONFIG_HPP

#include "Location_config.hpp"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <cctype>
#include <stdint.h>

//CLASS SERVER QUI PREND TOUT LES VARIABLES ET LES BLOCS LOCATIONS

class Location_config;

class Server_Config {
private:
    std::string _host;                           // Ex: "127.0.0.1", "0.0.0.0"
    int _port;                                   // Ex: 8080
    std::vector<std::string> _serverNames;       // Ex: ["localhost", "site.com"]
    std::string _root;                               // Ex: "/var/www/html"
    std::vector<std::string> _index;                 // Ex: ["index.html", "index.htm"]
    bool _autoindex;                                 // Listing de répertoire
    std::map<std::string, std::string> _cgiHandlers; // Ex: ".php", ".py" -> "/usr/bin/php-cgi"
    std::map<int, std::string> _errorPages;      // Ex: {404 -> "/errors/404.html"}
    size_t _clientMaxBodySize;                     // Taille max globale
    std::vector<Location_config> _locations;            // Liste des locations
    // Location_config* _defaultLocation;         // Location par défaut "/"

public:
    Server_Config();
    ~Server_Config();

    // Getters
    const std::string& getHost() const;
    int getPort() const;
    const std::vector<std::string>& getServerNames() const;
    const std::string& getRoot() const;
    const std::vector<std::string>& getIndex() const;
    bool isAutoindexEnabled() const;
    const std::map<std::string, std::string>& getCgiHandlers() const;
    const std::map<int, std::string>& getErrorPages() const;
    size_t getClientMaxBodySize() const;
    const std::vector<Location_config>& getLocations() const;
    
    // Setters


    void setHost(const std::string& host);
    void setPort(int port);
    void addServerName(const std::string& name);
    void setRoot(const std::string& root);
    void addIndex(const std::string& index);
    void setAutoindex(bool value);
    void setCgi(const std::string& extension, const std::string& path);
    void setErrorPage(int code, const std::string& path);
    void setClientMaxBodySize(size_t size);
    void addLocation(const Location_config& location);

    // Utilitaires
    void print() const;
    const Location_config* findLocation(const std::string& uri) const;
    // std::string getErrorPage(int code) const;
    // const Location_config* matchLocation(const std::string& uri) const;
    // bool matchesServerName(const std::string& host) const;
};

#endif