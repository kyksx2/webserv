#include "../include/Global_Config.hpp"
#include "../include/parsing.hpp"

Global_Config::Global_Config(const std::string& filepath) {
    parsing(filepath);
}

// void Config::buildFromTree(const std::vector<ConfigNode>& nodes) {
//     for (size_t i = 0; i < nodes.size(); i++) {
//         if (nodes[i].directive == "server") {
//             Server_Config server = buildServer(nodes[i]);
//             _servers.push_back(server);
//         }
//     }
// }

Server_Config Global_Config::buildServer(const ConfigNode& node)
{
    std::stringstream   ss;
    Server_Config       server;
    int                 code;

    for (std::vector<ConfigNode>::const_iterator it = node.children.begin(); 
         it != node.children.end(); ++it)
    {
        const ConfigNode& child = *it; 

        if (child.directive == "listen")
        {
            std::vector<std::string> parts = modifyArgListen(child);
            if (parts.size() == 2)
            {
                ss << parts[1];
                ss >> code;
                server.setHost(parts[0]);
                server.setPort(code);
                ss.clear();
            }
            else if (parts.size() == 1) {
                ss << parts[0];
                ss >> code;
				server.setHost("0.0.0.0");
                server.setPort(code);
                ss.clear();
			}
            else
                throw std::runtime_error("Erreur : Probleme de port");
        }
        if (child.directive == "server_name")
        {
            for (std::vector<std::string>::const_iterator vec_it = child.arguments.begin();
                 vec_it != child.arguments.end(); ++vec_it) {
                server.addServerName(*vec_it);
            }
        }
		if (child.directive == "error_page"){
                ss << child.arguments[0];
                ss >> code;
			    server.setErrorPage(code, child.arguments[1]);
                ss.clear();
		}
		if (child.directive == "client_max_body_size")
            server.setClientMaxBodySize(parseBodySize(child.arguments[0]));
        
        if (child.directive == "location")
        {
            server.addLocation(buildLocation(child));
        }
    }
    return (server);
}

Location_config Global_Config::buildLocation(const ConfigNode& node)
{
    Location_config location;
    std::stringstream   ss;
    int code = 0;

        for (std::vector<ConfigNode>::const_iterator it = node.children.begin(); 
         it != node.children.end(); ++it)
    {
        const ConfigNode& child = *it; 

        if (child.directive == "root")
            location.setRoot(child.arguments[0]);
        if (child.directive == "index")
        {
            for (size_t i = 0; i < child.arguments.size(); ++i)
                location.addIndex(child.arguments[i]);
        }
        if (child.directive == "allow_methods")
        {
            for (size_t i = 0; i < child.arguments.size(); ++i)
                location.addAllowedMethod(child.arguments[i]);
        }
        if (child.directive == "autoindex")
        {
            if (child.arguments[0] == "on")
                location.setAutoindex(true);
            else if (child.arguments[0] == "off")
                location.setAutoindex(false);
        }
        if (child.directive == "upload_store")
            location.setUploadStore(child.arguments[0]);
        if (child.directive == "cgi_handler")
            location.setCgi(child.arguments[0], child.arguments[1]);
        if (child.directive == "return")
        {
            ss << child.arguments[0];
            ss >> code;
            if (child.arguments.size() == 2)
                location.setRedirect(code, child.arguments[1]);
            else if (!isStringDigit(child.arguments[0]))
                location.setRedirect(0, child.arguments[0]);
            else if (isStringDigit(child.arguments[0]))
                location.setRedirect(code, NULL);
        }
        if (child.directive == "client_max_body_size")
            location.setClientMaxBodySize(parseBodySize(child.arguments[0]));
    }
    return (location);
}

void    Global_Config::parsing(const std::string& filepath)
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
        // std::cout << "--- Serveurs charges : " << _servers.size() << " ---" << std::endl; //! il faut croire que le vector est ok ici
        // for (size_t i = 0; i < _servers.size(); i++)
        // {
        //     _servers[i].print(); // ou _servers[i]->print() si ce sont des pointeurs
        // }
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
    
}

const std::vector<Server_Config>& Global_Config::getConfVect() const {
    return (this->_servers);
}

/*-----------------------UTILS-----------------------------*/

void Global_Config::print() const {
    std::cout << "=== Configuration ===" << std::endl;
    std::cout << "File: " << _configFilePath << std::endl;
    std::cout << "Servers (" << _servers.size() << "):" << std::endl;
    
    for (size_t i = 0; i < _servers.size(); i++) {
        _servers[i].print();
        std::cout << std::endl;
    }
}
