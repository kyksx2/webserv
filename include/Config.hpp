#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server_config.hpp"

enum TokenType {
    KEYWORD,      // server, location, listen, etc.
    VALUE,        // 443, /var/www/html, etc.
    OPEN_BRACE,   // {
    CLOSE_BRACE,  // }
    SEMICOLON     // ;
};

struct Token {
    TokenType type;
    std::string value;  // Le contenu textuel du token
    int line;           // Numéro de ligne (utile pour les erreurs)
};


struct ConfigNode {
    std::string directive;                    // "server", "location", "listen"
    std::vector<std::string> arguments;       // ["80"], ["/api"]
    std::vector<ConfigNode> children;         // blocs imbriqués
};

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