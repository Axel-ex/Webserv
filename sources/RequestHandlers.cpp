/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmoutinh <tmoutinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 09:47:14 by Axel              #+#    #+#             */
/*   Updated: 2024/07/23 23:23:02 by tmoutinh         ###   ########.fr       */
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
#include <cstdlib>

// =============================================================================
//                               ABSTRACT HANDLER
// =============================================================================
ARequestHandler ::ARequestHandler(void) : _next(NULL) {}

ARequestHandler ::~ARequestHandler(void) {}

void ARequestHandler ::handleRequest(const Request& request, Response& response)
{
    if (request.getProtocol() != "HTTP/1.1")
        return createErrorResponse(BAD_REQUEST, response);

    if (_canProcess(request))
        processRequest(request, response);
    else if (_next)
        _next->handleRequest(request, response);
    else
        createErrorResponse(BAD_REQUEST, response);
}

void ARequestHandler::setNextHandler(ARequestHandler* next) { _next = next; }

void ARequestHandler::createOkResponse(std::string resource,
                                       Response& response) const
{
    std::map<std::string, std::string>::iterator it;
    std::string headers;

    it = Config::getResources().find(resource);
    response.setBody(it->second);
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(response.getBody().length()) +
        "\r\n\r\n";
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
    headers = "HTTP/1.1 " + toString(error_code) + " " +
              it->second.reason + "\r\n";
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + toString(it->second.body.length()) +
               "\r\n\r\n";
    response.setHeaders(headers);
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
        return (createErrorResponse(NOT_FOUND, response));

    response.setBody(it->second);
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(response.getBody().length()) +
        "\r\n\r\n";
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

std::string PostRequestHandler ::_getContentType(const std::string& headers) const
{
    std::string content_type_key = "Content-Type: ";
    size_t content_type_pos = headers.find(content_type_key);
    if (content_type_pos == std::string::npos)
        return ("");
    content_type_pos += content_type_key.size();
    size_t content_type_end_pos;
    if (headers.find("\r\n", content_type_pos) < headers.find(";", content_type_pos))
        content_type_end_pos = headers.find("\r\n", content_type_pos);
    else 
        content_type_end_pos = headers.find(";", content_type_pos);
    std::string content_type =
        headers.substr(content_type_pos, content_type_end_pos - content_type_pos);
    return (content_type);
}

std::string PostRequestHandler ::_getFileContent(const std::string& body,
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

void PostRequestHandler::createResponse(std::string resource,
                                       Response& response) const
{
    std::string headers;
    std::string page;
    std::string decoded;
    std::string content;
    
    for (size_t i = 0; i < resource.length(); ++i) 
    {
        if (resource[i] == '+')
            decoded += ' ';
        else if (resource[i] == '%' && i + 2 < resource.length()) 
        {
            std::string hexValue = resource.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::strtol(hexValue.c_str(), NULL, 16));
            decoded += decodedChar;
            i += 2; // Skip the next two characters
        }
        else
            decoded += resource[i];
    }
    while (decoded.find("=") != std::string::npos)
    {
        decoded = decoded.substr(decoded.find("=") + 1);
        content += "<p>" + decoded.substr(0, decoded.find("& ")) + "</p>";
        decoded = decoded.substr(decoded.find("& ") + 2);
    }
    page = "<!DOCTYPE html><html><head><title>Passed: \
            </title></head><body><h1>"+content+"</h1>";
    response.setBody(page);
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(response.getBody().length()) +
        "\r\n\r\n";
    response.setHeaders(headers);
}

void PostRequestHandler::processRequest(const Request& request,
                                        Response& response) const
{
    std::string content_type = _getContentType(request.getHeaders());
    if (content_type.empty())
    {
        createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't get content type"));
    }
    std::string boundary = _getBoundary(request.getHeaders());
    if (boundary.empty() && content_type == "multipart/form-data")
    {
        createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't get content boundaries"));
    }
    
    std::string file_content;
    if (content_type == "multipart/form-data")
        file_content = _getFileContent(request.getBody(), boundary);
    else if (content_type == "application/x-www-form-urlencoded")
        file_content = request.getBody();
    std::string file_name = _getFileName(request.getBody());
    if (file_content.empty() || (file_name.empty() && content_type == "multipart/form-data"))
    {
        createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't process the file"));
    }

    if (content_type == "multipart/form-data")
    {
        // Write the file content to the file
        _createDir("uploads");
        file_name = "uploads/" + file_name;
        std::ofstream ofs(file_name.c_str(),
                        std::ios_base::out | std::ios_base::trunc);
        if (!ofs)
            return (Log::log(WARNING, "Couldn't write to file"));
        ofs << file_content;
        createOkResponse("posted", response);
    }
    else
        createResponse(file_content, response);
}

// =============================================================================
//                               DELETE
// =============================================================================
bool DeleteRequestHandler ::_canProcess(const Request& request) const
{
    return (request.getMethod() == "DELETE");
}

std::string DeleteRequestHandler ::_getPath(const std::string& resource) const
{
    std::string path = /*Append the root from the config file*/ /*+*/"/home/tmoutinh/WorkStation/Webserv" + resource;
    if (path[path.size() - 1] == '/')
        path = path.substr(0, path.size() - 1);
    return (path);
}

void DeleteRequestHandler ::processRequest(const Request& request,
                                           Response& response) const
{
    (void)response;
    std::string path = _getPath(request.getResource());
    struct stat info;

    if (path == "" || path == "root from config file")
    {
        createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't get the path"));
    }
    if (stat(path.c_str(), &info) != 0)
    {
        std::cout << "inside!!" << std::endl;
        createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "problem with path"));
    }
    else 
    {
        if (S_ISREG(info.st_mode))
        {
            if (remove(path.c_str()) != 0)
            {
                // Error handling if the file could not be deleted
                createErrorResponse(BAD_REQUEST, response);
                return (Log::log(WARNING, "Failed to delete file"));
            }
        }   
        else if (S_ISDIR(info.st_mode))
        {
            if (remove(path.c_str()) != 0)
            {
                // Error handling if the directory could not be deleted
                createErrorResponse(BAD_REQUEST, response);
                return (Log::log(WARNING, "Failed to delete file"));
            }
        }
        else
            createErrorResponse(BAD_REQUEST, response);
    }
    std::cout << "path.c_str" << path.c_str() << std::endl;
    createOkResponse("deleted", response);
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
