#include "Global_Config.hpp"
#include "parsing.hpp"

bool isStringDigit(std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!isdigit(str[i]))
            return (false);
    }
    return (true);
}

size_t parseBodySize(const std::string &value)
{
    size_t number = 0;
    size_t multiplier = 0;

    if (value.empty())
        throw std::runtime_error("Error : format listen invalide (host:port)");
    std::stringstream ss(value);
    ss >> number;
    if (ss.fail() || number < 0)
        throw std::runtime_error("Error : Invalid body size format");
    std::string unit;
    ss >> unit;
    for (size_t i = 0; i < unit.size(); ++i)
        unit[i] = std::tolower(unit[i]);
    if (unit.empty() || unit == "b")
        multiplier = 1;
    else if (unit == "k" || unit == "kb")
        multiplier = 1024;
    else if (unit == "m" || unit == "mb")
        multiplier = 1024 * 1024;
    else if (unit == "g" || unit == "gb")
        multiplier = 1024ULL * 1024 * 1024;
    else
        throw std::runtime_error("Unite inconnue : " + unit);
    if (number > SIZE_MAX / multiplier)
        throw std::runtime_error("Body size exceeds maximum allowed");
    return static_cast<size_t>(number * multiplier);
}

std::vector<std::string> modifyArgListen(const ConfigNode& node)
{
    if (node.arguments.empty())
        return std::vector<std::string>();
    const std::string& arg = node.arguments[0];
    size_t pos = arg.find(':');
    if (pos == std::string::npos)
        return node.arguments;
    std::string host = arg.substr(0, pos);
    std::string port = arg.substr(pos + 1);
    if (host.empty() || port.empty())
        throw std::runtime_error("Error : format listen invalide (host:port)");

    std::vector<std::string> parts;
    parts.push_back(host);
    parts.push_back(port);
    return parts;
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
                throw std::runtime_error("Error : port vide"); // Cas "127.0.0.1:"
            if (!isStringDigit(parts[1]))
                throw std::runtime_error("Error : port non digit");
            portStr = parts[1];
        }
        else
            portStr = parts[0];
        if (portStr.empty())
            throw std::runtime_error("Error : port vide");

        int code;
        std::stringstream ss(portStr);
        if (!(ss >> code) || code < 1 || code > 65535) {
            throw std::runtime_error("Error : port invalide");
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
