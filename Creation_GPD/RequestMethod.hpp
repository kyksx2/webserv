/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMethod.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:52:38 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/12 15:54:05 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTMETHOD_HPP
# define REQUESTMETHOD_HPP
#include "GetRequest.hpp"
#include "PostRequest.hpp"
#include "DeleteRequest.hpp"

std::vector<std::string>	split(const std::string &chaine, char delimiteur);

HTTPResponse	RequestCreation(std::string	buffer);
HTTPRequest	*get_creation(std::vector<std::string> &v);
HTTPRequest	*delete_creation(std::vector<std::string> &v);
HTTPRequest	*post_creation(std::vector<std::string> &v);

#endif