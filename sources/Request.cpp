/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 09:28:18 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 11:58:27 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include <sstream>
#include <string>

Request :: Request(std::string buffer)
{
	std::stringstream stream(buffer);
	std::string line;

	stream >> _method >> _resource >> _protocol;

	while (std::getline(stream, line) && !line.empty())
		_headers += line + "\n";
	if (!_headers.empty() && _headers.back() == '\n')
		_headers.pop_back();
	
	std::stringstream body_stream;
	body_stream << stream.rdbuf();
	_body = body_stream.str();
}

Request :: ~Request(void){}

const std::string &Request :: getMethod(void) const { return (_method);}

const std::string &Request :: getResource(void) const { return (_resource);}

const std::string &Request :: getProtocol(void) const { return (_protocol);}

const std::string &Request :: getHeaders(void) const { return (_headers);}

const std::string &Request :: getBody(void) const { return (_body);}


