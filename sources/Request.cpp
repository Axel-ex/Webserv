/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 09:28:18 by Axel              #+#    #+#             */
/*   Updated: 2024/10/11 11:54:26 by Axel             ###   ########.fr       */
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
    while (std::getline(stream, line) && line != "\r")
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
        { // Check and remove trailing '\r'
            line.erase(line.size() - 1);
        }
        _headers += line + "\r\n";
    }

    // Remove the extra "\r\n" added at the end of headers
    if (!_headers.empty() && _headers.size() >= 2)
        _headers.erase(_headers.size() - 2);

    // Parse the body
    std::stringstream body_stream;
    body_stream << stream.rdbuf();
    _body = body_stream.str();
	
	//extract host name for the headers
    size_t host_pos = _headers.find("Host: ");
    if (host_pos == std::string::npos)
        _host = "";
    host_pos += 6; // move past the "Host: "
    size_t return_pos = _headers.find('\r', host_pos);
    _host = _headers.substr(host_pos, return_pos - host_pos);
}

Request ::~Request(void) {}

const std::string& Request ::getMethod(void) const { return (_method); }

const std::string& Request::getHost(void) const {return (_host);}

const std::string& Request ::getResource(void) const { return (_resource); }

const std::string& Request ::getProtocol(void) const { return (_protocol); }

const std::string& Request ::getHeaders(void) const { return (_headers); }

const std::string& Request ::getBody(void) const { return (_body); }
