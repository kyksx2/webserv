#include <../include/webserv.hpp>

WebServ::WebServ(std::string& conf) {
    std::vector<servConf> config_server;

    confParser parser(conf);
    config_server = parser.parse();
    for (size_t i = 0; i < config_server.size(); i++) {
        Server* new_server = new Server(config_server[i]);
        this->servers.push_back(new_server);
    }
}

WebServ::~WebServ() {}