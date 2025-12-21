#ifndef CLIENT_H
#define CLIENT_H
#include "Server.hpp"

class Client {
 public:
   Client(int fd, Server* find_server);
   ~Client();
   Client(const Client& src);
   Client&  operator=(const Client& src);

   void appendRequest(const char* request, int size);
  //  bool completeRequest();
  //  void parseRequest();
  //  void generateResponse();
  //  int isKeepAlive();
   void clearState();

   std::string& getResponseBuffer();
  //  int getClientFd();
   size_t getDataSent();

   void setDataSent(int n);
    void setResponseBuffer(std::string& response);

 private:
   int client_fd;
   size_t data_sent; 
   Server* dad_serv;
   std::string requestBuffer;
   std::string responseBuffer;
  //  httpRequest request;
  //  httpResponse response;
};

#endif