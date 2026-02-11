/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 13:05:43 by kjolly            #+#    #+#             */
/*   Updated: 2026/02/10 15:47:50 by kjolly           ###   ########.fr       */
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

int main(int ac, char **av) {
    if (ac == 2 || ac == 1) {
        std::string conf;
        if (ac == 2)
            conf = av[1];
        signal(SIGINT, handle_sig);
        signal(SIGQUIT, handle_sig);
        signal(SIGPIPE, SIG_IGN);
        try
        {
            WebServ serv(conf);
            std::cout << std::endl << "----------------------------------------" << std::endl;
            serv.run();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            return (1);
        }
    }
    else {
        std::cerr << "Usage: ./webserv <.conf>" << std::endl;
        return (1);
    }
    return (0);
}
