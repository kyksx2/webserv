/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest2.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 15:47:17 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/15 16:53:53 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

static	std::vector<std::string>	split(const std::string &chaine, char delimiteur)
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

HTTPRequest::HTTPRequest(std::string &buffer)
{
	std::stringstream ss(buffer);
	std::string line;
	std::getline(ss, line);

	std::vector<std::string> firstline = split(line, ' ');
	if (firstline.size() < 3)
		throw std::runtime_error("400 Bad Request");

	std::string target = firstline[1];
	std::string version = firstline[2];

	// séparateur query
	size_t pos = target.find('?');
	if (pos != std::string::npos)
	{		//separe le maillon avant/apres "?"
		m_target = target.substr(0, pos);
		m_query = target.substr(pos + 1);
	}
	else
		m_target = target;

	if (m_target.empty())
		throw std::runtime_error("400 Bad Request");

	// nettoyage du \r final
	if (!version.empty() && version.back() == '\r')
		version.pop_back();

	m_version = version;
	if (m_version != "HTTP/1.0" && m_version != "HTTP/1.1")
		throw std::runtime_error("501 Not Implemented");

	while (std::getline(ss, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.empty())
			break;

	// chercher le séparateur clé/valeur
		size_t pos = line.find(':');
		if (pos == std::string::npos)
			throw std::runtime_error("400 Bad Request");

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		// trim espace(s) au début de la valeur
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		// gérer headers multiples (concat avec ", ")
		if (m_headers.count(key))
			m_headers[key] += ", " + value;
		else
			m_headers[key] = value;
	}
	// !body
}