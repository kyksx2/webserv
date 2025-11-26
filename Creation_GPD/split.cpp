/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 16:26:53 by yzeghari          #+#    #+#             */
/*   Updated: 2025/11/25 16:27:23 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestMethod.hpp"

std::vector<std::string>	split(const std::string &chaine, char delimiteur)
{
	std::vector<std::string>	v;
	std::stringstream ss(chaine);
	std::string	sous_chaine;

	while(std::getline(ss, sous_chaine, delimiteur))
	{
		v.push_back(sous_chaine);
	}
	return (v);
}