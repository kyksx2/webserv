/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:39:08 by yzeghari          #+#    #+#             */
/*   Updated: 2025/12/12 15:39:24 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTRequest_HPP
# define  POSTRequest_HPP

#include "HTTPRequest.hpp"

void	split_path(std::string path, std::string &dir, std::string &file);


class PostRequest : public HTTPRequest
{
	public:
		PostRequest(std::vector<std::string> &v);
		~PostRequest();
		HTTPResponse	generateResponse();
};
#endif