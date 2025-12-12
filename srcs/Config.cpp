#include "Config.hpp"
#include "parsing.hpp"

Config::Config(const std::string& filepath) {
    parsing(filepath);
}

void Config::buildFromTree(const std::vector<ConfigNode>& nodes) {
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].directive == "server") {
            Server_Config server = buildServer(nodes[i]);
            _servers.push_back(server);
        }
    }
}

Server_Config Config::buildServer(const ConfigNode& node)
{
    Server_Config server;
    int           code;

    for (std::vector<ConfigNode>::const_iterator it = node.children.begin(); 
         it != node.children.end(); ++it)
    {
        const ConfigNode& child = *it; 

        if (child.directive == "listen")
        {
            std::vector<std::string> parts = modifyArgListen(node);
            // std::cout << child.arguments.size();
            if (parts.size() == 2)
            {
                server.setHost(parts[0]);
                server.setPort(std::atoi(parts[1].c_str()));
            }
            else if (parts.size() == 1) {
				server.setHost("0.0.0.0");
                server.setPort(std::atoi(parts[1].c_str()));
			}
            else
                throw std::runtime_error("Erreur : Probleme de port");
        }
        if (child.directive == "server_name")
			server.addServerName(child.arguments[0]);
		if (child.directive == "error_page"){
            std::stringstream ss;
            ss >> code;
            std::cout << code << std::endl;
            // (child.arguments[0].c_str(), "%d", &code);
			server.setErrorPage(code, child.arguments[1]);
		}
		// if ()
    }
    return (server);
}

void    Config::parsing(const std::string& filepath)
{
    try
    {
        Parsing parsing(filepath);

        const ConfigNode tree = parsing.getTree();
        for (std::vector<ConfigNode>::const_iterator it = tree.children.begin(); 
            it != tree.children.end(); ++it)
        {
            const ConfigNode& child = *it; 
            _servers.push_back(buildServer(child));
        }
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
}
