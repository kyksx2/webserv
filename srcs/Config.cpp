#include "Config.hpp"
#include "parsing.hpp"

Config::Config() {
    //
}

Config::Config(const std::string& filepath) {
    parsing(filepath);
}

void    Config::parsing(const std::string& filepath)
{
    try
    {
        std::string fileContent = readFile(filepath);
        fileContent = cleanContent(fileContent);
        std::vector<Token> token = tokeniseContent(fileContent);
	    ConfigNode node = parse(token);
        validateNode(node);
	    print_node(node, 0);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
