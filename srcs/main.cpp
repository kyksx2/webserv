/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnolent <tnolent@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 13:05:43 by kjolly            #+#    #+#             */
/*   Updated: 2026/01/19 11:09:10 by tnolent          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serv_handler/Webserv.hpp"

//! gerer les timeout -> 60secondes sans activites = deconnection

volatile int signal_running = 1;

void handle_sig(int sig) {
    (void)sig;
    std::cout << "Receive signal: shut down the server." << std::endl;
    signal_running = 0;
}

// Main de timothee
int main(int ac, char **av)
{
    std::string const file_path(av[1]);
    if (ac == 2)
    {
        Global_Config config(file_path);
        config.print();
        const std::vector<Server_Config> server = config.getConfVect();
        const Location_config *fLoc = server[0].findLocation("/uploadcaca");
        fLoc->print();
    }
    else
        std::cout << "Pas de fichier de configuration";
}

// main pour kiki
// int main(int ac, char **av) {
//     if (ac == 2 || ac == 1) {
//         std::string conf;
//         if (ac == 2)
//             conf = av[1];
//         else
//             conf = "../config/default.conf";
//         signal(SIGINT, handle_sig);
//         signal(SIGQUIT, handle_sig);
//         signal(SIGPIPE, SIG_IGN);
//         try
//         {
//             WebServ serv(conf);
//             std::cout << std::endl << "----------------------------------------" << std::endl;
//             // serv.printEverythings();
//             serv.run();
//         }
//         catch(const std::exception& e)
//         {
//             std::cerr << "Error: " << e.what() << '\n';
//             return (1);
//         }
//     }
//     else {
//         std::cerr << "Usage: ./webserv <.conf>" << std::endl;
//         return (1);
//     }
//     return (0);
// }

//! main de test pour yasser
// int main() {
// 	std::string requet = "POST /index.html HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 10\r\n\r\nHelloworld";
// 	Client client;

// 	client.setRequest(requet);
// 	if (client.completeRequest())
// 	{
// 		client.printHeader();
// 		std::cout << "______________response_______" << std::endl;
// 		client.generateBufferResponse();
// 		client.printBufferResponse();
// 		std::cout << "_____________________________" << std::endl;
// 		std::cout << "request complete" << std::endl;
// 	}
// 	else {
// 		client.printHeader();
// 		std::cout << "request incomplete" << std::endl;
// 	}
// 	return (0);
// }
