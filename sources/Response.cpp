/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 15:00:56 by Axel              #+#    #+#             */
/*   Updated: 2024/10/03 11:48:27 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/RequestHandlers.hpp"
#include <string>

Response ::Response(const Request& request, Config& server_config)
    : _server_config(server_config)
{
    /* The request handling is design as a "chain of responsability" pattern*/
    /* https://www.geeksforgeeks.org/chain-responsibility-design-pattern/ */
    /* Create handlers for each request type */
    // CgiRequestHandler cgi_handler;
    GetRequestHandler get_handler;
    PostRequestHandler post_handler;
    DeleteRequestHandler delete_handler;

    /* Link the handlers together */
    // cgi_handler.setNextHandler(&get_handler);
    get_handler.setNextHandler(&post_handler);
    post_handler.setNextHandler(&delete_handler);
    delete_handler.setNextHandler(NULL);

    /* Pass the request to the handler of the chain */
    // cgi_handler.handleRequest(request, *this);
    get_handler.handleRequest(request, *this);
}

Response ::~Response(void) {}

const std::string& Response::getBody(void) const { return (_body); }

const std::string& Response::getHeaders(void) const { return (_headers); }

const std::string Response::getResponseBuffer(void) const
{
    return (_headers + _body);
}

void Response ::setHeaders(const std::string& headers) { _headers = headers; }

void Response ::setBody(const std::string& body) { _body = body; }

const Config& Response::getServerConfig(void) const { return _server_config; }
