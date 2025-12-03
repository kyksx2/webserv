#include "Config.hpp"
#include "parsing.hpp"

enum TokenType		determineType(const std::string& word)
{
	if (word == "server" || word == "location")
		return (KEYWORD);
	else
		return (VALUE);
}

ConfigNode parseBlock(const std::vector<Token>& tokens, size_t& i)
{
    ConfigNode node;

	if (tokens[i].type == KEYWORD)
		node.directive = tokens[i++].value;

	while (i < tokens.size() && tokens[i].type != OPEN_BRACE && tokens[i].type != SEMICOLON)
	{
		if (tokens[i].type == VALUE)
			node.arguments.push_back(tokens[i].value);
	}

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
