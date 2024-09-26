/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 15:00:56 by Axel              #+#    #+#             */
/*   Updated: 2024/09/26 14:20:41 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/RequestHandlers.hpp"
#include <string>
#include <sstream>

Response ::Response(const Request &request)
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

void Response::setCookie(const Cookie &cookie)
{
	_cookies.push_back(cookie);
}

Response ::~Response(void) {}

const std::string &Response::getBody(void) const { return (_body); }

const std::string &Response::getHeaders(void) const { return (_headers); }

const std::string Response::getResponseBuffer(void) const
{
	std::ostringstream response;
	std::string headers = _headers;
    if (headers.size() >= 2 && headers.substr(headers.size() - 2) == "\r\n")
    {
        // Remove the final "\r\n" from _headers
        headers = headers.substr(0, headers.size() - 2);
    }

    // Add the headers
    response << headers;
	std::vector<Cookie>::const_iterator it = _cookies.begin();
	for (; it != _cookies.end(); it++)
	{
		Cookie cookie = *it;
		response << "Set-Cookie: " << cookie.name << "=" << cookie.value;
		if (!cookie.expires.empty())
			response << "; Expires=" << cookie.expires;
		if (!cookie.maxAge.empty())
			response << "; Max-Age=" << cookie.maxAge;
		// if (!cookie.domain.empty())           <<-------------- NOT VALID FOR EXAMPLE.COM OR LOCALHOST
		// 	response << "; Domain=" << cookie.domain;
		if (!cookie.path.empty())
			response << "; Path=" << cookie.path;
		if (cookie.secure)
			response << "; Secure";
		if (cookie.httpOnly)
			response << "; HttpOnly";
		response << "\r\n";
	}
	response << "\r\n";
	response << _body;
	return (response.str());
}

void Response ::setHeaders(const std::string &headers) { _headers = headers; }

void Response ::setBody(const std::string &body) { _body = body; }
