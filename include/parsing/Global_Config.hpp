#ifndef GLOBAL_CONFIG_HPP
#define GLOBAL_CONFIG_HPP

#include "Server_config.hpp"
#include "parsing.hpp"

// CLASSE CONFIG QUI PREND TOUT LES SERVEURS DANS UN VECTOR
class Global_Config{
private:
    std::vector<Server_Config> _servers;
    std::string _configFilePath;

public:
    Global_Config();
    explicit Global_Config(const std::string& filepath);
    ~Global_Config();

    // Parsing
    void parsing(const std::string& filepath);
    
    // Getters
    const std::vector<Server_Config>& getConfVect() const;
    
    // Utilitaires
    Location_config buildLocation(const ConfigNode& node, Server_Config &server);
    void buildServer(const ConfigNode& node);
    void print() const;
};


#endif