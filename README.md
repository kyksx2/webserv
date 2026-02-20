*This project has been created as part of the 42 curriculum by kjolly, tnollent, yzeghari.*

# Webserv - HTTP/1.1 Server in C++ 98

## Description
Webserv is a 42 school project. The goal is to rewrite a functional HTTP server from scratch, inspired by **NGINX**. 

The main challenge consists of managing multiple clients simultaneously in a **non-blocking** way. We chose to use the `epoll` system call to handle a high volume of concurrent connections efficiently.

The server is capable of:
* Parsing and using a custom configuration file.
* Handling HTTP methods: `GET`, `POST`, and `DELETE`.
* Executing **CGI** scripts (Python, Bash, etc.) to generate dynamic content.
* Managing file uploads and providing an **autoindex** feature for directories.

## Instructions
### Step 1: Compilation
This project is written in **C++ 98**. You can compile the server using the provided Makefile at the root of the repository:

make

### Step 2: Execution
To launch the server, run the executable with a configuration file as an argument:
Bash

./webserv [path_to_config_file]

Note: If no path is provided, the server will attempt to use our default configuration file located in the configs/ folder.

### Step 3: Testing via Terminal
You can verify the server's behavior using another terminal with curl or telnet:

Using Curl (bash): curl -v [host]:[port]

Using Telnet (bash): telnet [host]:[port]

Note: When using Telnet, you must manually type the HTTP request (e.g., GET / HTTP/1.1) followed by the Host header and an empty line to receive a response.

### Step 4: Testing via Browser
To view your index.html or test the autoindex feature, enter the server's address in your browser:
http://[host]:[port]

## Ressources:
https://jameshfisher.com/2017/04/05/set_socket_nonblocking/
https://www.scottklement.com/rpg/socktut/introserver.html#INTROTOSERVERS
https://suchprogramming.com/epoll-in-3-easy-steps/
https://www.ionos.fr/digitalguide/hebergement/aspects-techniques/requete-http/
https://www.ionos.fr/digitalguide/hebergement/aspects-techniques/apercu-des-codes-de-statut-http-principaux/
https://www.alimnaqvi.com/blog/webserv
https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages
https://youtu.be/oYBgV474Udc?si=DGml_TGPRyQ3jFuM
https://datatracker.ietf.org/doc/html/rfc7230
https://datatracker.ietf.org/doc/html/rfc2616
https://fr.wikipedia.org/wiki/Hypertext_Transfer_Protocol
https://fr.wikipedia.org/wiki/Common_Gateway_Interface

## Technical Features
* **I/O Multiplexing**: Uses a single-threaded design to handle multiple clients simultaneously. This ensures the server remains responsive and never blocks while processing concurrent requests.
* **Error Handling**: Supports custom error pages for various HTTP status codes, providing clear feedback for malformed or failed requests.
* **CGI Implementation**: Utilizes `fork()` and `execve()` to execute external scripts. The child process runs the script and returns its output, allowing the server to handle dynamic content.
* **Resilience**: Engineered for high availability, the server is designed to handle edge cases and malformed data without crashing, remaining active until explicitly stopped.

## Use of IA
* Debugging
* Refactoring
* Documentation