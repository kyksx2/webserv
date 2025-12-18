#ifndef SERVCONF_H
#define SERVCONF_H
#include "webserv.hpp"
#include <iostream>

class ServConf {
 public:
    ServConf() : port(0), host("") {};
    ~ServConf(){};
    int port;
    std::string host;
 private:
};

#endif