#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <cctype>

// CLASSE LOCATION QUI PREND TOUTES LES VARIABLES DE LOCATION

class Location_config {
private:
    std::string _path;                               // Ex: "/", "/api", "/uploads"
    std::string _root;                               // Ex: "/var/www/html"
    std::vector<std::string> _index;                 // Ex: ["index.html", "index.htm"]
    std::vector<std::string> _allowedMethods;        // Ex: ["GET", "POST"]
    bool _autoindex;                                 // Listing de répertoire
    std::string _uploadStore;                        // Où stocker les uploads
    std::map<std::string, std::string> _cgiHandlers; // Ex: ".php", ".py" -> "/usr/bin/php-cgi"
    std::pair<int, std::string> _redirect;           // Ex: {301, "/new-path"}
    size_t _clientMaxBodySize;                       // Taille max du body

public:
    Location_config();
    // Location_config(const std::string& path);
    
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
    void print() const;
    // bool isMethodAllowed(const std::string& method) const;
    // bool isCgiRequest(const std::string& filename) const;
};

#endif