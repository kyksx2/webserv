/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creation_request.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:50:59 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/09 15:57:09 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestMethod.hpp"

//! 400 Bad Request A RAJOUTER && la recuperation des config du serv, besoin pour les methodes

//Met les attribut dans les variable pour faciliter la creation des obj
void	get_atr(std::vector<std::string> v,
				std::string &target,
				std::string &query,
				std::string &version,
				std::map<std::string, std::string> &headers)
{
	std::vector<std::string>::iterator	it = v.begin();
	it++;//skip le 1er mot

	size_t pos = (*it).find('?');// target/query   | verifie la presence de l argument optionel
	if (pos != std::string::npos)
	{         //separe le maillon avant/apres "?"
		target = (*it).substr(0, pos);
		query = (*it).substr(pos + 1, ((*it).size() - (pos + 1)));
	}
	else
	{
		target = (*it);
	}
	if (it != v.end())//Version
	{
		version = (*it);
		it++;
	}
	for (it; it != v.end(); ++it) // headers
	{
		std::string	key = (*it);
		// if ((key.find("\r\n")) == std::string::npos) // verifie que key ne contient pas /r/n
		//!je pense que c est deja verifer lors du parcour de l iterator
		it++;
		if (it != v.end())
		{
			std::string	value = (*it);
			while ((value.find("\r\n") == std::string::npos) && it != v.end())
			{
				value += " ";
				value += (*it);
				it++;
			}
			if (headers.find(key) != headers.end()) // verifie existe deja si oui ajoute ", valeur"
				headers[key] += ", " + value;
			else
				headers[key] = value;

		}
	}
}

HTTPRequest	*get_creation(std::vector<std::string> v)
{
	std::string	target;
	std::string	query;
	std::string version;
	std::map<std::string, std::string> headers;
	get_atr(v,target, query, version, headers);
	return (new GetRequest(target, query, version, headers));
}

HTTPRequest	*delete_creation(std::vector<std::string> v)
{
	std::string	target;
	std::string	query;
	std::string version;
	std::map<std::string, std::string> headers;
	get_atr(v,target, query, version, headers);
	return (new DeleteRequest(target, query, version, headers));
}

HTTPRequest	*post_creation(std::vector<std::string> v)
{
	std::string	target;
	std::string	query;
	std::string version;
	std::map<std::string, std::string> headers;
	get_atr(v,target, query, version, headers);
	return (new PostRequest(target, query, version, headers));
}

//Cree la bonne class en fonction du premier mot de la requete | renvoi exception si inconnue
HTTPRequest *RequestCreation(std::string buffer)
{
	std::vector<std::string>	v_request;
	int	i;
	std::string method[] = {"GET", "POST", "DELETE"};
	HTTPRequest *(*ft_method[])(std::vector<std::string>) = {
		get_creation,
		delete_creation,
		post_creation
	};

	v_request = split(buffer, ' ');

	for (i = 0; i < 3; i++)
	{
		if (method[i] == v_request[0])
			return ft_method[i](v_request);
	}
	throw std::runtime_error("Error: The method is either incorrectly written or not implemented.");
}
