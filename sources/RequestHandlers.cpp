/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 09:47:14 by Axel              #+#    #+#             */
/*   Updated: 2024/05/24 21:04:48 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandlers.hpp"
#include "../includes/Config.hpp"
#include <iostream>
#include <map>

/* ABSTRACT REQUEST HANDLER*/
ARequestHandler ::ARequestHandler(void) : _next(nullptr) {}

ARequestHandler ::~ARequestHandler(void) {}

// TODO: check protocol of request here
void ARequestHandler ::handleRequest(const Request& request, Response& response)
{
	if (request.getProtocol() != "HTTP/1.1")
		return createErrorResponse(BAD_REQUEST, response);

    if (canProcess(request))
        processRequest(request, response);
    else if (_next)
        _next->handleRequest(request, response);
    else
        createErrorResponse(BAD_REQUEST, response);
}

void ARequestHandler::setNextHandler(ARequestHandler* next) { _next = next; }

//TODO: createOkReponse 
void ARequestHandler ::createErrorResponse(int error_code,
                                           Response& response) const
{
    std::map<int, t_error>::iterator it;
    std::string headers;

    // codes we want to handle have to exist
    it = Config::getErrors().find(error_code);

    response.setBody(it->second.body);
    headers = "HTTP/1.1 " + std::to_string(error_code) + " " +
              it->second.reason + "\r\n";
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + std::to_string(it->second.body.length()) +
               "\r\n\r\n";
    response.setHeaders(headers);
}

/* CONCRETE IMPLEMENTATION OF THE HANDLERS */
/* GET */
bool GetRequestHandler ::canProcess(const Request& request) const
{
    return (request.getMethod() == "GET" && request.getMethod().find(".cgi") == std::string::npos);
}

void GetRequestHandler ::processRequest(const Request& request,
                                        Response& response) const
{
    std::map<std::string, std::string>::iterator it;
    std::string headers;
	
    it = Config::getResources().find(request.getResource());
    if (it == Config::getResources().end())
        return (createErrorResponse(NOT_FOUND, response));

    response.setBody(it->second);
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + std::to_string(response.getBody().length()) +
        "\r\n\r\n";
    response.setHeaders(headers);
}

/* POST*/
bool PostRequestHandler ::canProcess(const Request &request) const
{
    return (request.getMethod() == "POST" && request.getResource().find(".cgi") == std::string::npos);
}

void PostRequestHandler ::processRequest(const Request& request,
                                         Response& response) const
{
	(void)request;
	(void)response;
    std::cout << "hello world from post" << std::endl;
}

/*DELETE*/
bool DeleteRequestHandler ::canProcess(const Request& request) const
{
    return (request.getMethod() == "DELETE");
}

void DeleteRequestHandler ::processRequest(const Request& request,
                                           Response& response) const
{
    (void)request;
    (void)response;
    std::cout << "hello world from delete" << std::endl;
}

/*CGI*/
bool CgiRequestHandler ::canProcess(const Request& request) const
{
    return (request.getResource().find(".cgi") != std::string::npos);
}

void CgiRequestHandler ::processRequest(const Request& request,
                                        Response& response) const
{
    (void)request;
    (void)response;
    std::cout << "hello world from cgi" << std::endl;
}
