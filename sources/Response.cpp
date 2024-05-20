/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 15:00:56 by Axel              #+#    #+#             */
/*   Updated: 2024/05/20 10:52:57 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/RequestHandlers.hpp"
#include <string>

Response ::Response(const Request& request)
{
    /* The request handling is design as a "chain of responsability" pattern*/
	/* https://www.geeksforgeeks.org/chain-responsibility-design-pattern/ */
    /* Create handlers for each request type */
    CgiRequestHandler cgi_handler;
    GetRequestHandler get_handler;
    PostRequestHandler post_handler;
    DeleteRequestHandler delete_handler;

    /* Link the handlers together */
    cgi_handler.setNextHandler(&get_handler);
    get_handler.setNextHandler(&post_handler);
    post_handler.setNextHandler(&delete_handler);

    /* Pass the request to the handler of the chain */
    cgi_handler.handleRequest(request, *this);
}

Response ::~Response(void) {}

const std::string& Response::getBody(void) const { return (_body); }

const std::string& Response::getHeaders(void) const { return (_headers); }

void Response ::setHeaders(const std::string& headers) { _headers = headers; }

void Response ::setBody(const std::string& body) { _body = body; }
