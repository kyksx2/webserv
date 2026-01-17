// #ifndef CGIHANDLER_H
// #define CGIHANDLER_H
// #include "Webserv.hpp"

// class CgiHandler {
//  public:
//     CgiHandler();
//     CgiHandler(const CgiHandler& src);
//     ~CgiHandler();

//     CgiHandler& operator=(const CgiHandler& src);

//     void initEnv();
//     void executeCgi();
//     char** envStringToChar();

//  private:
//    //  httprequest request; il faut un moyen d'acceder au body de la requette
//     std::map<std::string, std::string> _env;
//     std::string binPath;
//     std::string scriptPath;
//     std::string response_cgi;
// };

// #endif