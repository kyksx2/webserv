#ifndef SERVEUR_CONFIG_HPP
#define SERVEUR_CONFIG_HPP

#include "Location_config.hpp"

//CLASS SERVER QUI PREND TOUT LES VARIABLES ET LES BLOCS LOCATIONS

class Server_Config {
private:
    std::string _host;                           // Ex: "127.0.0.1", "0.0.0.0"
    int _port;                                   // Ex: 8080
    std::vector<std::string> _serverNames;       // Ex: ["localhost", "site.com"]
    std::map<int, std::string> _errorPages;      // Ex: {404 -> "/errors/404.html"}
    size_t _clientMaxBodySize;                   // Taille max globale
    std::vector<Location_config> _locations;            // Liste des locations
    // Location_config* _defaultLocation;                  // Location par défaut "/"

public:
    Server_Config();
    ~Server_Config();

    // Getters
    const std::string& getHost() const;
    int getPort() const;
    const std::vector<std::string>& getServerNames() const;
    const std::map<int, std::string>& getErrorPages() const;
    size_t getClientMaxBodySize() const;
    const std::vector<Location_config>& getLocations() const;
    
    // Setters
    void setHost(const std::string& host);
    void setPort(int port);
    void addServerName(const std::string& name);
    void setErrorPage(int code, const std::string& path);
    void setClientMaxBodySize(size_t size);
    void addLocation(const Location_config& location);

    // Utilitaires
    // std::string getErrorPage(int code) const;
    // const Location_config* matchLocation(const std::string& uri) const;
    // bool matchesServerName(const std::string& host) const;
};

#endif