#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

enum TokenType {
    KEYWORD,      // server, location, listen, etc.
    VALUE,        // 443, /var/www/html, etc.
    OPEN_BRACE,   // {
    CLOSE_BRACE,  // }
    SEMICOLON     // ;
};

class Location {
private:
    std::string _path;                           // Ex: "/", "/api", "/uploads"
    std::string _root;                           // Ex: "/var/www/html"
    std::vector<std::string> _index;             // Ex: ["index.html", "index.htm"]
    std::vector<std::string> _allowedMethods;    // Ex: ["GET", "POST"]
    bool _autoindex;                             // Listing de répertoire
    std::string _uploadStore;                    // Où stocker les uploads
    std::string _cgiExtension;                   // Ex: ".php", ".py"
    std::string _cgiPath;                        // Ex: "/usr/bin/php-cgi"
    std::pair<int, std::string> _redirect;       // Ex: {301, "/new-path"}
    size_t _clientMaxBodySize;                   // Taille max du body

public: 
    Location(const std::string& path);
    
    // Getters
    const std::string& getPath() const;
    const std::string& getRoot() const;
    const std::vector<std::string>& getIndex() const;
    const std::vector<std::string>& getAllowedMethods() const;
    bool isAutoindexEnabled() const;
    const std::string& getUploadStore() const;
    const std::string& getCgiExtension() const;
    const std::string& getCgiPath() const;
    const std::pair<int, std::string>& getRedirect() const;
    size_t getClientMaxBodySize() const;
    
    // Setters
    void setRoot(const std::string& root);
    void addIndex(const std::string& index);
    void addAllowedMethod(const std::string& method);
    void setAutoindex(bool value);
    void setUploadStore(const std::string& path);
    void setCgi(const std::string& extension, const std::string& path);
    void setRedirect(int code, const std::string& url);
    void setClientMaxBodySize(size_t size);
    
    // Utilitaires
    // bool isMethodAllowed(const std::string& method) const;
    // bool isCgiRequest(const std::string& filename) const;
};

class Config {
private:
    std::string _host;                           // Ex: "127.0.0.1", "0.0.0.0"
    int _port;                                   // Ex: 8080
    std::vector<std::string> _serverNames;       // Ex: ["localhost", "site.com"]
    std::map<int, std::string> _errorPages;      // Ex: {404 -> "/errors/404.html"}
    size_t _clientMaxBodySize;                   // Taille max globale
    std::vector<Location> _locations;            // Liste des locations
    Location* _defaultLocation;                  // Location par défaut "/"

public:
    Config();
    ~Config();
    
    // Getters
    const std::string& getHost() const;
    int getPort() const;
    const std::vector<std::string>& getServerNames() const;
    const std::map<int, std::string>& getErrorPages() const;
    size_t getClientMaxBodySize() const;
    const std::vector<Location>& getLocations() const;
    
    // Setters
    void setHost(const std::string& host);
    void setPort(int port);
    void addServerName(const std::string& name);
    void setErrorPage(int code, const std::string& path);
    void setClientMaxBodySize(size_t size);
    void addLocation(const Location& location);
    
    // Utilitaires
    std::string getErrorPage(int code) const;
    const Location* matchLocation(const std::string& uri) const;
    bool matchesServerName(const std::string& host) const;
};

#endif