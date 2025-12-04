/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMethod.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:52:38 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/03 15:14:36 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTMETHOD_HPP
# define REQUESTMETHOD_HPP
#include "GetRequest.hpp"
#include "PostRequest.hpp"
#include "DeleteRequest.hpp"

std::vector<std::string>	split(const std::string &chaine, char delimiteur);

void	get_atr(std::vector<std::string> v,
				std::string &target,
				std::string &query,
				std::string &version,
				std::map<std::string, std::string> &headers);

HTTPRequest	*RequestCreation(std::string	buffer);
#endif