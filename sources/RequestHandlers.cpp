/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 09:47:14 by Axel              #+#    #+#             */
/*   Updated: 2024/05/26 16:11:27 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandlers.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <string>

ARequestHandler ::ARequestHandler(void) : _next(nullptr) {}

ARequestHandler ::~ARequestHandler(void) {}

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

void ARequestHandler::createOkResponse(std::string resource, Response& response) const
{
    // TODO: either we have all the resources static or we have a html template
    // we could modify depending on what we want to communicate to the client
	std::map<std::string, std::string>::iterator it;
	std::string headers;

	it = Config::getResources().find(resource);
	response.setBody(it->second);
    headers = "HTTP/1.1 200 OK\r\n"; // Changed from HTTPS/1.1 to HTTP/1.1
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + std::to_string(response.getBody().length()) + "\r\n";
    headers += "\r\n"; // Ensure correct separation between headers and body
    response.setHeaders(headers);

}

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
    return (request.getMethod() == "GET" &&
            request.getMethod().find(".cgi") == std::string::npos);
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
bool PostRequestHandler ::canProcess(const Request& request) const
{
    return (request.getMethod() == "POST" &&
            request.getResource().find(".cgi") == std::string::npos);
}

std::string PostRequestHandler ::_getBoundary(const std::string& headers) const
{
    std::string boundary_key = "boundary=";
    size_t boundary_pos = headers.find(boundary_key);
    if (boundary_pos == std::string::npos)
        return ("");
    boundary_pos += boundary_key.size();
    size_t boundary_end_pos = headers.find("\r\n", boundary_pos);
    std::string boundary =
        "--" + headers.substr(boundary_pos, boundary_end_pos - boundary_pos);
    return (boundary);
}

std::string
PostRequestHandler ::_getFileContent(const std::string& body,
                                     const std::string& boundary) const
{
    size_t start = body.find(boundary);
    if (start == std::string::npos)
        return ("");
    start += boundary.size();

    // Skip past the headers for the file content
    size_t header_end = body.find("\r\n\r\n", start);
    if (header_end == std::string::npos)
        return ("");
    header_end += 4; // Move past the \r\n\r\n

    // Find the end of the file content
    size_t end = body.find(boundary, header_end);
    if (end == std::string::npos)
        return ("");

    std::string file_content = body.substr(
        header_end,
        end - header_end - 2); // Subtract 2 to remove the trailing \r\n
    return (file_content);
}

std::string PostRequestHandler ::_getFileName(const std::string& body) const
{
    size_t filename_pos = body.find("filename=\"");
    if (filename_pos == std::string::npos)
        return ("");
    filename_pos += 10; // Move past "filename=\""
    size_t filename_end = body.find("\"", filename_pos);
    std::string file_name =
        body.substr(filename_pos, filename_end - filename_pos);
    Log::log(DEBUG, file_name);
    return (file_name);
}

void PostRequestHandler::processRequest(const Request& request,
                                        Response& response) const
{
    std::string body = request.getBody();

    (void)response;

    std::string boundary = _getBoundary(request.getHeaders());
    if (boundary.empty())
    {
        createErrorResponse(BAD_REQUEST, response);
        return (Log::log(ERROR, "couldn't get content boundaries"));
    }

    std::string file_content = _getFileContent(request.getBody(), boundary);
    std::string file_name = _getFileName(request.getBody());
    if (file_content.empty() || file_name.empty())
    {
        createErrorResponse(400, response);
        return (Log::log(ERROR, "couldn't process the file"));
    }

    // Write the file content to the file
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::trunc);
    if (!ofs)
        return (Log::log(ERROR, "Couldn't write to file"));
    ofs << file_content;
	createOkResponse("posted", response);
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
