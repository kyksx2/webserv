#include "Config.hpp"
#include "parsing.hpp"

void print_node(ConfigNode &node, int layer)
{
	int n = layer;
	std::string tabs(n, '\t');
	size_t i = 0;
	if (!node.directive.empty())
		std::cout << tabs << layer << " " << "[ Directive: " << node.directive << "]";

	if (!node.arguments.empty())
		std::cout << " " << "[ Arguments: ";
	for (std::vector<std::string>::iterator it = node.arguments.begin(); it != node.arguments.end(); ++it)
		std::cout << *it << " ";
	std::cout << "] : " << node.arguments.size() << std::endl;

	while (i < node.children.size())
	{
		print_node(node.children[i], layer + 1);
		i++;
	}
}

enum TokenType		determineType(const std::string& word)
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

std::vector<std::string> modifyArgListen(const ConfigNode& node)
{
    if (node.arguments.empty()) {
        return std::vector<std::string>();
    }

    std::string arg = node.arguments[0];
    std::vector<std::string> parts;

    size_t pos = arg.find(':');
    if (pos != std::string::npos) {
        parts.push_back(arg.substr(0, pos));        // avant les :
        parts.push_back(arg.substr(pos + 1));       // après les :
        return parts;
    }
    return node.arguments;
}

bool caseByCase_directive(const ConfigNode& node)
{
    if (node.directive == "listen")
    {
        if (node.arguments.size() != 1)
            return false;

        std::vector<std::string> parts = modifyArgListen(node);
        
        if (parts.empty()) {
            return false;
        }

        // Vérifier que la chaîne n'est pas vide et contient uniquement des chiffres
        std::string portStr = parts[0];
        if (portStr.empty()) {
            std::cerr << "Erreur: port vide\n";
            return false;
        }

        // Gérer les exceptions de std::stoi
        try {
            int port = std::stoi(portStr);
            if (port < 1 || port > 65535) {
                std::cerr << "Erreur: port invalide " << port << "\n";
                return false;
            }
        } catch (const std::invalid_argument&) {
            std::cerr << "Erreur: port n'est pas un nombre valide\n";
            return false;
        } catch (const std::out_of_range&) {
            std::cerr << "Erreur: port hors limites\n";
            return false;
        }
    }

    return true;
}

bool validateNode(const ConfigNode& node)
{
    bool has_listen;
    bool has_root;

    if (node.directive == "server")
    {
        for (const auto& child : node.children)
        {
            if (child.directive == "listen")
                has_listen = true;
            if (child.directive == "root")
                has_root = true;
        }
        if (!has_listen)
            throw std::runtime_error("Erreur : le serveur n'a pas de port attribué");
        if (!has_root)
            throw std::runtime_error("Erreur : le serveur n'a pas de repertoire attribué");
    }

    caseByCase_directive(node);
    // Valider récursivement les enfants
    for (const auto& child : node.children) {
        if (!validateNode(child))
            return false;
    }
    return true;
}

ConfigNode parseBlock(const std::vector<Token>& tokens, size_t& i)
{
    ConfigNode node;

	if (tokens[i].type == KEYWORD)
		node.directive = tokens[i++].value;
    else
        throw std::runtime_error("Erreur : mauvaise directive : [" + tokens[i].value + "]");

	while (i < tokens.size() && tokens[i].type != OPEN_BRACE && tokens[i].type != SEMICOLON)
	{
		if (tokens[i].type == VALUE)
			node.arguments.push_back(tokens[i].value);
        else if (tokens[i].type == KEYWORD)
            throw std::runtime_error("Probleme de syntaxe");
        i++;
	}
    // Récursivité pour chercher tout les enfants
	if (tokens[i].type == OPEN_BRACE)
	{
		i++;
		while (i < tokens.size() && tokens[i].type != CLOSE_BRACE)
			node.children.push_back(parseBlock(tokens, i));
		i++;
	}

	else if (tokens[i].type == SEMICOLON)
		i++;
	return (node);
}

// Fait un arbre de syntaxe abstraite avec les tokens du vector Token pour structurer les blocks servers et locations
ConfigNode parse(const std::vector<Token>& tokens)
{
    ConfigNode root;
    root.directive = "root";
    
    size_t i = 0;
    while (i < tokens.size()) {
        root.children.push_back(parseBlock(tokens, i));
    }
    return root;
}

std::vector<Token> tokeniseContent(const std::string& fileContent)
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
                tokens.push_back({KEYWORD, word, line});
                word.clear();
            }
            tokens.push_back({OPEN_BRACE, "{", line});
        }
        else if (c == '}') {
            if (!word.empty()) {
                tokens.push_back({VALUE, word, line});
                word.clear();
            }
            tokens.push_back({CLOSE_BRACE, "}", line});
        }
        else if (c == ';') {
            if (!word.empty()) {
                tokens.push_back({VALUE, word, line});
                word.clear();
            }
            tokens.push_back({SEMICOLON, ";", line});
        }
        else if (isspace(c)) {
            if (!word.empty()) {
                tokens.push_back({determineType(word), word, line});
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    
    return tokens;
}

std::string readFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string cleanContent(const std::string& fileContent)
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
