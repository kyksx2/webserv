/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjolly <kjolly@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 13:05:43 by kjolly            #+#    #+#             */
/*   Updated: 2025/12/10 16:45:44 by kjolly           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"

//! add a signal handler

int main(/*int ac, char **av*/) {
    // if (ac == 2 || ac == 1) {
    //     std::string conf;
    //     if (ac == 2)
    //         conf = av[1];
    //     else
    //         conf = "../config/default.conf";
    //     try
    //     {
            WebServ serv/*(conf)*/;
            serv.run();
    //     }
    //     catch(const std::exception& e)
    //     {
    //         std::cerr << "Error: " << e.what() << '\n';
    //         return (1);
    //     }
    // }
    // else {
    //     std::cerr << "Usage: ./webserv <.conf>" << std::endl;
    //     return (1);
    // }
    return (0);
}