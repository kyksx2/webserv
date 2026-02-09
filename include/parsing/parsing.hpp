#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <unistd.h>

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
	Token(TokenType t, const std::string& v, int l) : type(t), value(v), line(l) {}
};


struct ConfigNode {
	std::string					directive;                    // "server", "location", "listen"
	std::vector<std::string>	arguments;       // ["80"], ["/api"]
	std::vector<ConfigNode>		children;         // blocs imbriqués
};

class Parsing {
	private:
		ConfigNode	_tree;
	public:
		Parsing(const std::string& filepath);

		void				errorPageCase(const ConfigNode& node);
		void				numberArgCase(const ConfigNode& node);
		void				methodCase(const ConfigNode& node);
		void				cgiCase(const ConfigNode& node);
		void				listenCase(const ConfigNode& node);
		void				validateNode(const ConfigNode& node);
		std::string			cleanContent(const std::string& fileContent);
		std::string			readFile(const std::string& filename);
		enum TokenType		determineType(const std::string& word);
		std::vector<Token>	tokeniseContent(const std::string& fileContent);
		ConfigNode			parseBlock(const std::vector<Token>& tokens, size_t& i);
		void				caseByCase_directive(const ConfigNode& node);
		bool				isValidIp(const std::string ipAdress);
		bool				validOctet(const std::string octet);

		/*-----------------Setter--------------------------*/
		ConfigNode	setTree(const std::vector<Token>& tokens);

		/*----------------Getter---------------------------*/
		const ConfigNode	getTree(void);
};

std::vector<std::string>	modifyArgListen(const ConfigNode& node);
void	print_node(ConfigNode &node, int layer);  // Pour afficher la config parsée
bool	isStringDigit(std::string str);
size_t	parseBodySize(const std::string &value);

#endif