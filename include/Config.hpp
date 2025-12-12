#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server_config.hpp"
#include "parsing.hpp"

// CLASSE CONFIG QUI PREND TOUT LES SERVEURS DANS UN VECTOR
class Config {
private:
    std::vector<Server_Config> _servers;
    std::string _configFilePath;

public:
    explicit Config(const std::string& filepath);
    
    // Parsing
    void parsing(const std::string& filepath);
    
    // // Getters
    // const std::vector<Server_Config>& getServers() const;
    // const std::string& getConfigFilePath() const;
    
    // // Utilitaires
    void buildFromTree(const std::vector<ConfigNode>& nodes);
    Server_Config buildServer(const ConfigNode& node);
    
};


#endif