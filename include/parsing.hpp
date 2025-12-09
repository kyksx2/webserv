#ifndef PARSING_HPP
#define PARSING_HPP

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

bool validateNode(const ConfigNode& node);
void print_node(ConfigNode &node, int layer);
std::string cleanContent(const std::string& fileContent);
std::string readFile(const std::string& filename);
enum TokenType		determineType(const std::string& word);
std::vector<Token> tokeniseContent(const std::string& fileContent);
ConfigNode parse(const std::vector<Token>& tokens);
ConfigNode parseBlock(const std::vector<Token>& tokens, size_t& i);

#endif