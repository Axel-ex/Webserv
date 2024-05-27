/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 09:28:18 by Axel              #+#    #+#             */
/*   Updated: 2024/05/27 06:37:28 by axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include <sstream>
#include <string>

Request::Request(std::string buffer)
{
    std::stringstream stream(buffer);
    std::string line;

    // Parse the request line
    std::getline(stream, line);
    std::istringstream request_line(line);
    request_line >> _method >> _resource >> _protocol;

    // Parse the headers
    while (std::getline(stream, line) && line != "\r") {
        if (!line.empty() && line[line.size() - 1] == '\r') { // Check and remove trailing '\r'
            line.erase(line.size() - 1);
        }
        _headers += line + "\r\n";
    }

    // Remove the extra "\r\n" added at the end of headers
    if (!_headers.empty() && _headers.size() >= 2) {
        _headers.erase(_headers.size() - 2);
    }

    // Parse the body
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


