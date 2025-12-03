#include "Config.hpp"
#include "parsing.hpp"

Config::Config() {
    //
}

Config::Config(const std::string& filepath) {
    parsing(filepath);
}


// void print_node(ConfigNode &node)
// {
// 	bool status = true;
// 	int i = 1;

// 	// while (status)
// 	{
// 		std::cout << i << std::endl;
// 		if (!node.directive.empty())
// 			std::cout << node.directive << std::endl;
// 		for (std::vector<ConfigNode>::iterator it = node.children.begin(); it != node.children.end(); ++it)
// 			std::cout << it->arguments << std::endl;
// 	}
// }

void    Config::parsing(const std::string& filepath)
{
    std::string fileContent = readFile(filepath);
    fileContent = cleanContent(fileContent);
    std::vector<Token> token = tokeniseContent(fileContent);
	// for (std::vector<Token>::iterator it = token.begin(); it != token.end(); ++it)
	// 	std::cout << it->type << " ";
	ConfigNode node = parse(token);
	// print_node(node);
}

