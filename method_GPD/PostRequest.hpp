/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:39:08 by yzeghari          #+#    #+#             */
/*   Updated: 2025/11/25 14:31:44 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTRequest_HPP
# define  POSTRequest_HPP

#include "HTTPRequest.hpp"

class PostRequest : public HTTPRequest
{
	public:
		PostRequest(/* args */);
		~PostRequest();
};
#endif