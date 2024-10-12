/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 09:47:14 by Axel              #+#    #+#             */
/*   Updated: 2024/10/12 11:00:30 by achabrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestHandlers.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include "../includes/Parser.hpp"
#include "../includes/utils.hpp"
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>

// =============================================================================
//                               ABSTRACT HANDLER
// =============================================================================
void ARequestHandler::initializeMimeTypes()
{
    mimeTypes[".ico"] = "image/x-icon";
    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".txt"] = "text/plain";
    // Add more MIME types as needed
}

ARequestHandler ::ARequestHandler(void) : _next(NULL) { initializeMimeTypes(); }

ARequestHandler ::~ARequestHandler(void) {}

void ARequestHandler ::handleRequest(const Request& request, Response& response)
{
    if (request.getProtocol() != "HTTP/1.1")
        return _createErrorResponse(BAD_REQUEST, response);
    if (_canProcess(request, response.getServerConfig().getRoutes()))
        processRequest(request, response);
    else if (_next)
        _next->handleRequest(request, response);
    else
        _createErrorResponse(METHOD_NOT_ALLOWED, response);
}

void ARequestHandler::setNextHandler(ARequestHandler* next) { _next = next; }

void ARequestHandler::_createOkResponse(std::string resource,
                                        Response& response) const
{
    std::string headers;

    (void)resource;
    response.setBody("<h1>" + resource + "<h1/>");
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(response.getBody().length()) + "\r\n\r\n";
    response.setHeaders(headers);
}

void ARequestHandler ::_createErrorResponse(int error_code,
                                            Response& response) const
{
    std::map<int, std::string>::const_iterator it;
    std::string headers;

    // codes we want to handle have to exist
    it = response.getServerConfig().getErrors().find(error_code);

    response.setBody(it->second);
    headers = "HTTP/1.1 " + toString(error_code) + " " +
              _getErrorReason(error_code) + "\r\n";
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + toString(it->second.length()) + "\r\n\r\n";
    response.setHeaders(headers);
}

std::string ARequestHandler::_getErrorReason(int error_code) const
{

    switch (error_code)
    {
    default:
        return ("Page Not Found");
    case NOT_FOUND:
        return ("Page Not Found");
    case BAD_REQUEST:
        return ("Bad Request");
    case INTERNAL_ERROR:
        return ("Internal Server Error");
    case FORBIDDEN:
        return ("Forbidden");
    case METHOD_NOT_ALLOWED:
        return ("Method not allowed");
    case PAYLOAD_LARGE:
        return ("Payload too large");
    }
}

/* CONCRETE IMPLEMENTATION OF THE HANDLERS */
// =============================================================================
//                               GET
// =============================================================================
bool GetRequestHandler ::_canProcess(const Request& request,
                                     const std::vector<Route>& routes) const
{
    Route best_route = ServerTools::getBestRoute(request, routes);
    return (!best_route.url.empty() && request.getMethod() == "GET");
}

std::string GetRequestHandler ::_get_file_content(std::string path,
                                                  Response& response) const
{
    std::stringstream buffer;
    std::ifstream file(path.c_str());
    std::string cont_type;

    if (!file.is_open())
        return ("");
    buffer << file.rdbuf();
    response.setBody(buffer.str());
    file.close();

    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos)
    {
        std::string extension = path.substr(idx);
        if (mimeTypes.find(extension) != mimeTypes.end())
        {
            return (mimeTypes.find(extension)->second);
        }
        else
            return ("");
    }
    else
        return ("");
}

void GetRequestHandler ::processRequest(const Request& request,
                                        Response& response) const
{
    Route best_route = ServerTools::getBestRoute(
        request, response.getServerConfig().getRoutes());
    if (!best_route.redirect_url.second.empty())
        return (_createRedirectResponse(response, best_route));

    std::string resource = request.getResource();
    std::string headers;
    std::string req_path;
    std::string cont_type;
    struct stat info;

    if (resource == best_route.url || resource == best_route.url + "/")
        req_path = best_route.root + "/" + best_route.index;
    else
        req_path = best_route.root + resource;

    if (stat((req_path).c_str(), &info) != 0)
    {
        _createErrorResponse(NOT_FOUND, response);
        return (Log::log(WARNING, "No such path or directory"));
    }
    if (S_ISREG(info.st_mode))
    {
        cont_type = GetRequestHandler::_get_file_content(req_path, response);
        if (cont_type.empty())
        {
            _createErrorResponse(NOT_FOUND, response);
            return (Log::log(WARNING, "couldn't get path"));
        }
        headers = "HTTP/1.1 200 OK\r\n";
        headers += "Content-Type: " + cont_type + "\r\n";
        headers += "Content-Length: " + toString(response.getBody().length()) +
                   "\r\n\r\n";
        response.setHeaders(headers);
    }
    else
    {
        if (best_route.autoindex)
            _createAutoIndexResponse(req_path, resource, response);
        else
        {
            _createErrorResponse(FORBIDDEN, response);
            return (Log::log(WARNING, "Is a directory"));
        }
    }
}

void GetRequestHandler::_createAutoIndexResponse(const std::string& true_path,
                                                 const std::string& route_path,
                                                 Response& response) const
{
    DIR* dir = opendir(true_path.c_str());
    if (!dir)
    {
        _createErrorResponse(FORBIDDEN, response);
        return Log::log(ERROR, "Could not open directory");
    }

    std::stringstream body;
    body << "<html><head><title>Index of " << route_path << "</title></head>";
    body << "<body><h1>Index of " << route_path << "</h1><ul>";

    struct dirent* entry;
    while ((entry = readdir(dir)))
    {
        // Skip "." and ".." directories
        if (std::string(entry->d_name) == "." ||
            std::string(entry->d_name) == "..")
            continue;

        // Build the relative path for each item (file or folder)
        std::string item_name = entry->d_name;
        std::string item_path = route_path + "/" + item_name;

        // Check if it's a directory (we want to append "/" for directories)
        struct stat item_stat;
        std::string full_item_path = true_path + "/" + item_name;
        if (stat(full_item_path.c_str(), &item_stat) == 0 &&
            S_ISDIR(item_stat.st_mode))
            item_name += "/";

        // Add the item to the HTML body
        body << "<li><a href=\"" << item_path << "\">" << item_name
             << "</a></li>";
    }

    body << "</ul></body></html>";
    closedir(dir);

    std::string html_content = body.str();
    response.setBody(html_content);

    std::string headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + toString(html_content.size()) + "\r\n\r\n";
    response.setHeaders(headers);
}

void GetRequestHandler::_createRedirectResponse(Response& response,
                                                Route& route) const
{
    std::string header =
        "HTTP/1.1 " + toString(route.redirect_url.first) + " Redirect\r\n";
    header += "Location: " + route.redirect_url.second + "\r\n\r\n";
    response.setHeaders(header);
}

// =============================================================================
//                               POST
// =============================================================================
bool PostRequestHandler ::_canProcess(const Request& request,
                                      const std::vector<Route>& routes) const
{
    Route best_route = ServerTools::getBestRoute(request, routes);
    return (!best_route.url.empty() && request.getMethod() == "POST");
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
PostRequestHandler ::_getContentType(const std::string& headers) const
{
    std::string content_type_key = "Content-Type: ";
    size_t content_type_pos = headers.find(content_type_key);
    if (content_type_pos == std::string::npos)
        return ("");
    content_type_pos += content_type_key.size();
    size_t content_type_end_pos;
    if (headers.find("\r\n", content_type_pos) <
        headers.find(";", content_type_pos))
        content_type_end_pos = headers.find("\r\n", content_type_pos);
    else
        content_type_end_pos = headers.find(";", content_type_pos);
    std::string content_type = headers.substr(
        content_type_pos, content_type_end_pos - content_type_pos);
    return (content_type);
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
            char decodedChar =
                static_cast<char>(std::strtol(hexValue.c_str(), NULL, 16));
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
            </title></head><body><h1>" +
           content + "</h1><br><br>";
    response.setBody(page);
    headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers +=
        "Content-Length: " + toString(response.getBody().length()) + "\r\n\r\n";
    response.setHeaders(headers);
}

bool PostRequestHandler::_bodySizeCheck(const Request& request,
                                        const Config& config) const
{
    unsigned long max_body_size = config.getMaxBodySize();

    return (request.getBody().size() <= max_body_size);
}

void PostRequestHandler::processRequest(const Request& request,
                                        Response& response) const
{
    std::string content_type = _getContentType(request.getHeaders());
    Config config = response.getServerConfig();

    const std::vector<Route>& routes = config.getRoutes();
    std::string upload_store = routes.begin()->upload_store;
    if (!_bodySizeCheck(request, config))
    {
        _createErrorResponse(PAYLOAD_LARGE, response);
        return (Log::log(WARNING, "Payload Too Large"));
    }
    if (upload_store.empty() || upload_store == "./" || upload_store == "/")
        upload_store = "uploads";
    if (content_type.empty())
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't get content type"));
    }
    std::string boundary = _getBoundary(request.getHeaders());
    if (boundary.empty() && content_type == "multipart/form-data")
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't get content boundaries"));
    }

    std::string file_content;
    if (content_type == "multipart/form-data" ||
        content_type == "application/json" ||
        content_type == "application/octet-stream")
        file_content = _getFileContent(request.getBody(), boundary);
    else // if (content_type == "application/x-www-form-urlencoded" ||
         // content_type = "text/plain")
        file_content = request.getBody();
    std::string file_name = _getFileName(request.getBody());
    if (file_content.empty() ||
        (file_name.empty() && content_type == "multipart/form-data"))
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't process the file"));
    }

    if (content_type == "multipart/form-data" ||
        content_type == "application/json" ||
        content_type == "application/octet-stream")
    {
        // Write the file content to the file
        _createDir(upload_store);
        std::string dir = upload_store + "/" + file_name;
        std::ofstream ofs(dir.c_str(),
                          std::ios_base::out | std::ios_base::trunc);
        if (!ofs)
            return (Log::log(WARNING, "Couldn't write to file"));
        ofs << file_content;
        if (!ofs)
            return (Log::log(WARNING,
                             "Failed to write contente to file: " + file_name));
        ofs.close();
        _createOkResponse(file_name + " Posted", response);
    }
    else
        createResponse(file_content, response);
}

// =============================================================================
//                               DELETE
// =============================================================================
bool DeleteRequestHandler ::_canProcess(const Request& request,
                                        const std::vector<Route>& routes) const
{
    Route best_route = ServerTools::getBestRoute(request, routes);
    return (!best_route.url.empty() && request.getMethod() == "DELETE");
}

std::string _GetUserAgent(std::string userAgent)
{
    size_t pos = userAgent.find("User-Agent:");
    std::string agent;

    if (pos != std::string::npos)
    {
        agent = userAgent.substr(pos + 12, userAgent.find("/", pos) - pos - 12);
        return (agent);
    }
    return ("");
}

std::string DeleteRequestHandler ::_getPath(const Request& request,
                                            Response& response) const
{
    Config config = response.getServerConfig();
    const std::vector<Route>& routes = config.getRoutes();
    std::string del_path;

    del_path =
        request.getResource(); //.substr(0, request.getResource().find("/"));
    if (routes.empty())
    {
        _createErrorResponse(BAD_REQUEST, response);
        return ("");
    }
    if (del_path.empty())
    {
        _createErrorResponse(BAD_REQUEST, response);
        return ("");
    }
    std::string path = routes.begin()->upload_store + "/" + del_path;
    if (path[path.size() - 1] == '/')
        path = path.substr(0, path.size() - 1);
    return (path);
}

void DeleteRequestHandler ::processRequest(const Request& request,
                                           Response& response) const
{
    struct stat info;
    std::string path;
    std::string user_agent;

    user_agent = _GetUserAgent(request.getHeaders());
    if (user_agent != "Chrome" && user_agent != "Firefox" && user_agent != "Mozilla" &&
        user_agent != "Safari" && user_agent != "Edge" && user_agent != "Opera")
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "Invalid delete request"));
    }
    path = _getPath(request, response);
    if (path == "")
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "couldn't get the path"));
    }
    if (stat(path.c_str(), &info) != 0)
    {
        _createErrorResponse(BAD_REQUEST, response);
        return (Log::log(WARNING, "No such path"));
    }
    else
    {
        if (S_ISREG(info.st_mode))
        {
            if (remove(path.c_str()) != 0)
            {
                // Error handling if the file could not be deleted
                _createErrorResponse(BAD_REQUEST, response);
                return (Log::log(WARNING, "Failed to delete file"));
            }
        }
        else if (S_ISDIR(info.st_mode))
        {
            if (remove(path.c_str()) != 0)
            {
                // Error handling if the directory could not be deleted
                _createErrorResponse(BAD_REQUEST, response);
                return (Log::log(WARNING, "Failed to delete file"));
            }
        }
        else
            _createErrorResponse(BAD_REQUEST, response);
    }
    _createOkResponse("deleted", response);
}
