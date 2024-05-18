/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 15:00:56 by Axel              #+#    #+#             */
/*   Updated: 2024/05/17 11:40:46 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/Config.hpp"
#include <map>
#include <string>

Response ::Response(const Request& request)
{
    if (request.getProtocol() != "HTTP/1.1")
    {
        _createErrorResponse(400);
        return;
    }
    if (request.getMethod() == "GET")
        _createGetResponse(request);
    else if (request.getMethod() == "POST")
        _createPostResponse();
    else if (request.getMethod() == "DELETE")
        _createDeleteResponse();
    else
        _createErrorResponse(400);
}

Response ::~Response(void) {}

const std::string& Response::getBody(void) const { return (_body); }

const std::string& Response::getHeaders(void) const { return (_headers); }

void Response::_createErrorResponse(int error_code)
{
    std::map<int, t_error>::iterator it;

    // codes we want to handle have to exist
    it = Config::getErrors().find(error_code);
    _headers = "HTTP/1.1 " + std::to_string(error_code) + " " +
               it->second.reason + "\r\n";
    _headers += "Content-Type: text/html\r\n";
    _headers += "Content-Length: " + std::to_string(it->second.body.length()) +
                "\r\n\r\n";
    _body = it->second.body;
}

void Response ::_createGetResponse(const Request& request)
{
    std::map<std::string, std::string>::iterator it;

    it = Config::getResources().find(request.getResource());
    if (it == Config::getResources().end())
    {
        _createErrorResponse(404);
        return;
    }

    _body = it->second;
    _headers = "HTTP/1.1 200 OK\r\n";
    _headers += "Content-Type: text/html\r\n";
    _headers +=
        "Content-Length: " + std::to_string(_body.length()) + "\r\n\r\n";
}

void Response ::_createPostResponse(void) {}
void Response ::_createDeleteResponse(void) {}
