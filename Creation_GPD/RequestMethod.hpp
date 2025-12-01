/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMethod.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:52:38 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/01 16:27:26 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTMETHOD_HPP
# define REQUESTMETHOD_HPP
#include "GetRequest.hpp"
#include "PostRequest.hpp"
#include "DeleteRequest.hpp"

enum HTTPStatusCode {
	OK = 200,
	Created = 201,
	Accepted = 202,
	MovedPermanently = 301,
	PermanentRedirect = 308,
	BadRequest = 400,
	Unauthorized = 401,
	Forbidden = 403,
	NotFound = 404,
	MethodNotAllowed = 405,
	RequestTimeout = 408,
	UnsupportedMediaType = 415,
	InternalServerError = 500,
	NotImplemented = 501
};

#include <sstream>
std::vector<std::string>	split(const std::string &chaine, char delimiteur);

void	get_atr(std::vector<std::string> v,
				std::string &target,
				std::string &query,
				std::string &version,
				std::map<std::string, std::string> &headers);

HTTPRequest	*RequestCreation(std::string	buffer);
#endif