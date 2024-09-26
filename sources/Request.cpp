/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 09:28:18 by Axel              #+#    #+#             */
/*   Updated: 2024/09/26 14:47:41 by ebmarque         ###   ########.fr       */
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


// =============================================================================
//                              Cookies Management
// =============================================================================

const std::string &Request::getCookie(const std::string &name) const
{
    static const std::string empty = "";
    std::map<std::string, std::string>::const_iterator it = _cookies.find(name);
    if (it != _cookies.end())
        return (it->second);
    return (empty);
}

void Request::parseCookie()
{
    size_t start = _headers.find("Cookie:");
    if (start != std::string::npos)
    {
        size_t end = _headers.find("\r\n", start);
        if (end == std::string::npos)
            end = _headers.length();

        std::string cookies = _headers.substr(start + 7, end - (start + 7));
        std::istringstream cookieStream(cookies);
        std::string cookie;

        while (std::getline(cookieStream, cookie, ';'))
        {
            size_t pos = cookie.find('=');
            if (pos != std::string::npos)
            {
                std::string name = cookie.substr(0, pos);
                std::string value = cookie.substr(pos + 1);
                _cookies[trim(name)] = trim(value);  // Remover espaços em branco
            }
        }
    }
}

// Função auxiliar para remover espaços em branco
std::string Request::trim(const std::string &str) const
{
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool Request::hasCookie(const std::string &name) const
{
    return _cookies.find(name) != _cookies.end();
}