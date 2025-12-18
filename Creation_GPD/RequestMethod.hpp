/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMethod.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:52:38 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/18 16:13:15 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTMETHOD_HPP
# define REQUESTMETHOD_HPP
#include "method_GPD/GetRequest.hpp"
#include "method_GPD/PostRequest.hpp"
#include "method_GPD/DeleteRequest.hpp"

#include <sstream>

HTTPResponse	RequestCreation(std::string	buffer, const Server& serv);
HTTPRequest	*get_creation(std::string &buffer, const Server& serv);
HTTPRequest	*delete_creation(std::string &buffer, const Server& serv);
HTTPRequest	*post_creation(std::string &buffer, const Server& serv);

#endif