#include "Config.hpp"
#include "parsing.hpp"

std::vector<std::string> modifyArgListen(const ConfigNode& node)
{
    if (node.arguments.empty()) {
        return std::vector<std::string>();
    }

    std::string arg = node.arguments[0];
    std::vector<std::string> parts;

    size_t pos = arg.find(':');
    if (pos != std::string::npos) {
        parts.push_back(arg.substr(0, pos));
        parts.push_back(arg.substr(pos + 1));

        return parts;
    }
    return node.arguments;
}

bool    Parsing::listenCase(const ConfigNode& node)
{
    if (node.directive == "listen")
    {
        if (node.arguments.size() != 1)
            return false;

        std::vector<std::string> parts = modifyArgListen(node);
        
        if (parts.empty()) {
            return false;
        }

        std::string portStr;
        if (parts.size() == 2)
        {
            if (parts[1].empty())
                throw std::runtime_error("Erreur : port vide"); // Cas "127.0.0.1:"
			// verifier digit
			// isdigit(parts[1]);
            portStr = parts[1];
        }
        else
            portStr = parts[0];
        if (portStr.empty())
            throw std::runtime_error("Erreur : port vide");

        int code;
        std::stringstream ss(portStr);
        if (!(ss >> code) || code < 1 || code > 65535) {
            throw std::runtime_error("Erreur : port invalide");
        }
    }
    return (true);
}

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
		print_node(node.children[i++], layer + 1);
}
