/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzeghari <yzeghari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:46:05 by yzeghari          #+#    #+#             */
/*   Updated: 2025/11/25 13:47:13 by yzeghari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETREQUEST_HPP
# define  GETREQUEST_HPP

#include "HTTPRequest.hpp"

class GetRequest : public HTTPRequest
{
	public:
		GetRequest();
		~GetRequest();
};
#endif