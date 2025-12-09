#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server_config.hpp"

// CLASSE CONFIG QUI PREND TOUT LES SERVEURS DANS UN VECTOR
class Config {
private:
    // std::vector<Server_Config> _servers;
    std::string _configFilePath;

public:
    Config();
    explicit Config(const std::string& filepath);
    
    // Parsing
    void parsing(const std::string& filepath);
    
    // // Getters
    // const std::vector<Server_Config>& getServers() const;
    // const std::string& getConfigFilePath() const;
    
    // // Utilitaires
    // const Server_Config* findServer(const std::string& host, int port) const;
    // const Server_Config* findServerByPort(int port) const;
    
    // // Debug
    // void print() const;  // Pour afficher la config parsée
};


#endif