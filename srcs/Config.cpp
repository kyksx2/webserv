#include "Config.hpp"
#include "parsing.hpp"

Config::Config() {
    //
}

Config::Config(const std::string& filepath) {
    parse(filepath);
}



void    Config::parse(const std::string& filepath)
{
    std::vector<Token> token;

    std::string fileContent = readFile(filepath);
    fileContent = cleanContent(fileContent);
    token = tokeniseContent(fileContent);
	for (std::vector<Token>::iterator it = token.begin(); it != token.end(); ++it)
		// std::cout << it->type << " ";
}

