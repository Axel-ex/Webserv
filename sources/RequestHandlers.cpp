/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 09:47:14 by Axel              #+#    #+#             */
/*   Updated: 2024/08/02 12:22:27 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandlers.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include "../includes/utils.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>

// =============================================================================
//                               ABSTRACT HANDLER
// =============================================================================
ARequestHandler ::ARequestHandler(void) : _next(NULL) {}

ARequestHandler ::~ARequestHandler(void) {}

void ARequestHandler ::handleRequest(const Request& request, Response& response)
{
    if (request.getProtocol() != "HTTP/1.1")
        return _createErrorResponse(BAD_REQUEST, response);
    if (_canProcess(request))
        processRequest(request, response);
    else if (_next)
        _next->handleRequest(request, response);
    else
        _createErrorResponse(BAD_REQUEST, response);
}

void ARequestHandler::setNextHandler(ARequestHandler* next) { _next = next; }

void ARequestHandler::_createOkResponse(std::string resource,
                                        Response& response) const
{
    std::map<std::string, std::string>::iterator it;
    std::string headers;

    it = Config::getResources().find(resource);
    response.setBody(it->second);
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(response.getBody().length()) + "\r\n\r\n";
    response.setHeaders(headers);
}

void ARequestHandler ::_createErrorResponse(int error_code,
                                            Response& response) const
{
    std::map<int, std::string>::iterator it;
    std::string headers;

    // codes we want to handle have to exist
    it = Config::getDefaultErrors().find(error_code);

    response.setBody(it->second);
    headers =
        "HTTP/1.1 " + toString(error_code) + " " + _getErrorReason(error_code) + "\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(it->second.length()) + "\r\n\r\n";
    response.setHeaders(headers);
}

std::string ARequestHandler::_getErrorReason(int error_code) const {

	switch (error_code) {
		default:
			return ("Page not found");
		case NOT_FOUND:
			return ("Page not found");
		case BAD_REQUEST:
			return ("Bad request");
		case INTERNAL_ERROR:
			return ("Internal server error");
	}
}

/* CONCRETE IMPLEMENTATION OF THE HANDLERS */
// =============================================================================
//                               GET
// =============================================================================
bool GetRequestHandler ::_canProcess(const Request& request) const
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
        return (_createErrorResponse(NOT_FOUND, response));

    response.setBody(it->second);
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(response.getBody().length()) + "\r\n\r\n";
    response.setHeaders(headers);
}

// =============================================================================
//                               POST
// =============================================================================
bool PostRequestHandler ::_canProcess(const Request& request) const
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

void PostRequestHandler ::_createDir(std::string dir_name) const
{
    struct stat info;

    if (stat(dir_name.c_str(), &info) != 0)
    {
        if (mkdir(dir_name.c_str(), 0777))
            Log::log(ERROR, "couldn't create " + dir_name + " directory");
    }
}

void PostRequestHandler::processRequest(const Request& request,
                                         Response& response) const
{
    std::string boundary = _getBoundary(request.getHeaders());
    if (boundary.empty())
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't get content boundaries"));
    }

    std::string file_content = _getFileContent(request.getBody(), boundary);
    std::string file_name = _getFileName(request.getBody());
    if (file_content.empty() || file_name.empty())
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't process the file"));
    }

    // Write the file content to the file
    _createDir("uploads");
    file_name = "uploads/" + file_name;
    std::ofstream ofs(file_name.c_str(),
                      std::ios_base::out | std::ios_base::trunc);
    if (!ofs)
        return (Log::log(WARNING, "Couldn't write to file"));
    ofs << file_content;
    _createOkResponse("posted", response);
}

// =============================================================================
//                               DELETE
// =============================================================================
bool DeleteRequestHandler ::_canProcess(const Request& request) const
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

// =============================================================================
//                               CGI
// =============================================================================
bool CgiRequestHandler ::_canProcess(const Request& request) const
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
