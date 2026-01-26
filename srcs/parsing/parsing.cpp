#include "parsing/parsing.hpp"
#include "parsing/Global_Config.hpp"

Parsing::Parsing(const std::string& filepath)
{
    std::string fileContent = readFile(filepath);
    fileContent = cleanContent(fileContent);
    std::vector<Token> token = tokeniseContent(fileContent);
	_tree = setTree(token);
    validateNode(_tree);
}

enum TokenType  Parsing::determineType(const std::string& word)
{
	if (word == "server" || word == "location" || word == "listen" ||
        word == "server_name" || word == "client_max_body_size" ||
        word == "index" || word == "error_page" || word == "allow_methods" ||
        word == "autoindex" || word == "return" || word == "cgi_handler" ||
        word == "root" || word == "upload_store")
		return (KEYWORD);
	else
		return (VALUE);
}

ConfigNode Parsing::parseBlock(const std::vector<Token>& tokens, size_t& i)
{
    ConfigNode node;

	if (tokens[i].type == KEYWORD)
		node.directive = tokens[i++].value;
    else {
        throw std::runtime_error("Error : mauvaise directive : [" + tokens[i].value + "]");
    }
	while (i < tokens.size() && tokens[i].type != OPEN_BRACE && tokens[i].type != SEMICOLON)
	{
		if (tokens[i].type == VALUE)
			node.arguments.push_back(tokens[i].value);
        else if (tokens[i].type == KEYWORD)
            throw std::runtime_error("Error : Probleme de syntaxe");
        i++;
	}
    // Récursivité pour chercher tout les enfants
	if (i < tokens.size() && tokens[i].type == OPEN_BRACE)
	{
		i++;
		while (i < tokens.size() && tokens[i].type != CLOSE_BRACE)
			node.children.push_back(parseBlock(tokens, i));
		i++;
	}
	else if (i < tokens.size() && tokens[i].type == SEMICOLON)
		i++;
	return (node);
}

// Fait un arbre de syntaxe abstraite avec les tokens du vector Token pour structurer les blocks servers et locations
ConfigNode Parsing::setTree(const std::vector<Token>& tokens)
{
    ConfigNode root;
    root.directive = "root_server";
    
    size_t i = 0;
    while (i < tokens.size()) {
        root.children.push_back(parseBlock(tokens, i));
    }
    return root;
}

std::vector<Token> Parsing::tokeniseContent(const std::string& fileContent)
{
    std::vector<Token> tokens;
    std::string word;
    int line = 1;
    
    for (size_t i = 0; i < fileContent.length(); i++) {
        char c = fileContent[i];
        
        if (c == '\n') {
            line++;
        }
        else if (c == '{') {
            if (!word.empty()) {
                tokens.push_back(Token(KEYWORD, word, line));
                word.clear();
            }
            tokens.push_back(Token(OPEN_BRACE, "{", line));
        }
        else if (c == '}') {
            if (!word.empty()) {
                tokens.push_back(Token(VALUE, word, line));
                word.clear();
            }
            tokens.push_back(Token(CLOSE_BRACE, "}", line));
        }
        else if (c == ';') {
            if (!word.empty()) {
                tokens.push_back(Token(VALUE, word, line));
                word.clear();
            }
            tokens.push_back(Token(SEMICOLON, ";", line));
        }
        else if (isspace(c)) {
            if (!word.empty()) {
                tokens.push_back(Token(determineType(word), word, line));
                word.clear();
            }
        }
        else
            word += c;
    }
    return tokens;
}

std::string Parsing::readFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Parsing::cleanContent(const std::string& fileContent)
{
    int i = 0;
    char inquote = 0;
    std::string result;

    while(fileContent[i])
    {
        if (strchr("\"'", fileContent[i]) && !inquote)
            inquote = fileContent[i];
        else if (inquote == fileContent[i])
            inquote = 0;
        if (strchr("#", fileContent[i]) && !inquote)
        {
            while (fileContent[i] && fileContent[i] != '\n')
                i++;
        }
        else
            result += fileContent[i++];
    }
    return (result);
}

/*--------------------------GETTERS---------------------------------------*/
const ConfigNode Parsing::getTree(void)
{
    return (_tree);
}

/*-------------------------HANDLE ERROR-----------------------------------*/

bool Parsing::validateNode(const ConfigNode& node)
{
    //voir les doublons
    if (node.directive == "server")
    {
        for (std::vector<ConfigNode>::const_iterator it = node.children.begin(); 
            it != node.children.end(); ++it)
        {
            const ConfigNode& child = *it; 
            if (child.directive == "server")
                throw std::runtime_error("Error : block serveur a l'interieur d'un block serveur");
        }
    }
    if (node.directive == "location")
    {
        for (std::vector<ConfigNode>::const_iterator it = node.children.begin(); 
            it != node.children.end(); ++it)
        {
            const ConfigNode& child = *it; 
            if (child.directive == "location")
                throw std::runtime_error("Error : block location a l'interieur d'un block location");
        }
    }
    caseByCase_directive(node);
    // Valider récursivement les enfants
    for (std::vector<ConfigNode>::const_iterator it = node.children.begin(); 
        it != node.children.end(); ++it)
    {
        const ConfigNode& child = *it; 
        if (!validateNode(child))
            return false;
    }
    return true;
}

void Parsing::caseByCase_directive(const ConfigNode& node)
{
    numberArgCase(node);
    if (node.directive == "listen")
        listenCase(node);
    if (node.directive == "error_page")
        errorPageCase(node);
    if (node.directive == "cgi_handler")
        cgiCase(node);
}

void    Parsing::cgiCase(const ConfigNode& node)
{
        if (!(node.arguments[0][0] == '.'))
            throw std::runtime_error("Error: L'extension du CGI ne commence pas par un point");
        if (access(node.arguments[1].c_str(), X_OK) != 0)
            throw std::runtime_error("Error: Le path du CGI n'est pas disponible");
        if (access(node.arguments[1].c_str(), R_OK | X_OK) != 0) 
            throw std::runtime_error("Error: Le path du CGI n'est pas executable");
}

void    Parsing::listenCase(const ConfigNode& node)
{
    int code;
    std::string portStr;
    std::vector<std::string> parts;

    {
        parts = modifyArgListen(node);
        if (parts.size() == 2)
        {
            if (!isStringDigit(parts[1]))
                throw std::runtime_error("Error : port non digit");
            portStr = parts[1];
            if (!isValidIp(parts[0]))
                throw std::runtime_error("Error : Adresse IP non valide");
        }
        else
        {
            if (!isStringDigit(parts[0]))
                throw std::runtime_error("Error : port non digit");
            portStr = parts[0];
        }
        std::stringstream ss(portStr);
        if (!(ss >> code) || code < 1 || code > 65535) {
            throw std::runtime_error("Error : port invalide");
        }
    }
}

void    Parsing::numberArgCase(const ConfigNode& node)
{
    std::stringstream error_msg;

    if (node.directive == "root" || node.directive == "autoindex" ||
        node.directive == "client_max_body_size" ||
        node.directive == "listen" || node.directive == "upload_store" ||
        node.directive == "location")
    {
        if (node.arguments.size() != 1){
            error_msg << "Error : directive '" << node.directive << "' should have only one argument";
            throw std::runtime_error(error_msg.str());
        }
    }
    if (node.directive == "error_page" && node.arguments.size() < 2)
        throw std::runtime_error("Error : directive 'error_page' should have at least two arguments");
    if (node.directive == "cgi_handler" && node.arguments.size() != 2)
        throw std::runtime_error("Error : directive 'cgi_handler' should have two arguments");
}

void    Parsing::errorPageCase(const ConfigNode& node)
{
    {
        int code = 0;
        if (node.arguments.size() != 2)
            throw std::runtime_error("Error : La directive error_page doit avoir deux");
        std::stringstream ss(node.arguments[0]);
        if (!isStringDigit(ss.str()))
            throw std::runtime_error("Error : Nombre non valide pour le code erreur.");
        ss >> code;
        if (code < 300 || code >= 600)
            throw std::runtime_error("Error : La directive error_page doit avoir un code erreur valid (entre 300 et 599)");
    }
}
