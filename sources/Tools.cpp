/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 13:22:10 by ebmarque          #+#    #+#             */
/*   Updated: 2024/10/11 10:53:44 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include "../includes/utils.hpp"
#include "../includes/Server.hpp"
#include <fstream>

std::string ServerTools::getMatch(const std::vector<Route>& routes,
                                  std::string& resource, std::string method)
{
    std::string match = "";
    for (size_t i = 0; i < routes.size(); i++)
    {
        std::vector<std::string>::const_iterator it = routes[i].methods.begin();
        for (; it != routes[i].methods.end(); it++)
        {
            if (*it == method)
                break;
        }
        if (it == routes[i].methods.end())
            continue;
        if (resource.find(routes[i].url) == 0 &&
            routes[i].url.size() > match.size())
            match = routes[i].url;
    }
    return (match);
}

Route ServerTools::getBestRoute(const Request& request,
                                const std::vector<Route>& routes)
{
    std::string resource = request.getResource();
    std::string match = getMatch(routes, resource, request.getMethod());
    Route emptyRoute;

    for (size_t i = 0; i < routes.size(); i++)
    {
        if (routes[i].url == match)
            return (routes[i]);
    }
    return (emptyRoute);
}

double ServerTools::getTime(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL))
        throw std::runtime_error("Function 'gettimeofday failed.\n");
    return (tv.tv_sec + (tv.tv_usec / 1e6));
}

bool ServerTools::MatchPort::operator()(Server& server) const
{
    // Check if the server has the matching port
    const std::vector<int>& ports = server.getConfig().getPorts();
    return std::find(ports.begin(), ports.end(), port_to_find) != ports.end();
}

unsigned int CgiTools::convertHex(const std::string& nb)
{
    unsigned int x;
    std::stringstream ss;
    ss << nb;
    ss >> std::hex >> x;
    return (x);
}

std::string CgiTools::getFileExtension(const std::string& url)
{
    size_t queryPos = url.find('?');

    std::string path =
        (queryPos != std::string::npos) ? url.substr(0, queryPos) : url;

    size_t dotPos = path.find_first_of('.');
    if (dotPos == std::string::npos)
        return ("");

    if (dotPos == path.size() - 1)
        return ("");
    path = path.substr(dotPos);
    path = path.substr(0, path.find_first_of("/#"));
    return (path);
}

bool CgiTools::isExtensionAllowed(
    const std::string& url, const std::vector<std::string>& cgi_extensions)
{
    std::string extension = CgiTools::getFileExtension(url);
    for (size_t i = 0; i < cgi_extensions.size(); ++i)
    {
        if (extension == cgi_extensions[i])
            return (true);
    }
    return (false);
}

int CgiTools::getStatusCode(int error_code)
{
    switch (error_code)
    {
    case ENOENT:
        return (404);
        break;
    case EACCES:
        return (403);
        break;
    case EINVAL:
        return (400);
        break;
    case EPERM:
        return (401);
        break;
    case EFAULT:
        return (500);
        break;
    case EEXIST:
        return (409);
        break;
    case ENOTDIR:
        return (404);
        break;
    case ETIMEDOUT:
        return (504);
        break;
    default:
        return (500);
        break;
    }
    return (500);
}

void CgiTools::sendErrorPage(int client_fd, std::string file_path,
                             int status_code)
{
    Log::log(DEBUG, "Using error page: " + toString(RED) + file_path +
                        toString(RESET));
    std::ifstream file(file_path.c_str());
    if (file.is_open())
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        std::string response_headers =
            "HTTP/1.1 " + toString(status_code) +
            " OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: " +
            toString(content.length()) +
            "\r\n"
            "Connection: close\r\n"
            "\r\n";

        send(client_fd, response_headers.c_str(), response_headers.length(), 0);
        send(client_fd, content.c_str(), content.length(), 0);
        file.close();
    }
}

void CgiTools::sendHttpErrorResponse(
    int client_fd, int error_code,
    const std::map<int, std::string>& error_pages)
{
    std::string error_message;
    int status_code = CgiTools::getStatusCode(error_code);
    std::string reason_phrase = "Internal Server Error";

    if (error_pages.find(status_code) != error_pages.end())
    {
        CgiTools::sendErrorPage(
            client_fd, error_pages.find(status_code)->second, status_code);
        return;
    }

    switch (status_code)
    {
    case 404:
        reason_phrase = "Not Found";
        error_message = "The requested resource was not found on this server.";
        break;
    case 403:
        reason_phrase = "Forbidden";
        error_message =
            "You do not have permission to access the requested resource.";
        break;
    case 400:
        reason_phrase = "Bad Request";
        error_message = "The server could not understand the request due to "
                        "invalid syntax.";
        break;
    case 401:
        reason_phrase = "Unauthorized";
        error_message = "Authentication is required and has failed or has not "
                        "yet been provided.";
        break;
    case 500:
        reason_phrase = "Internal Server Error";
        error_message = "The server encountered an internal error and was "
                        "unable to complete your request.";
        break;
    case 409:
        reason_phrase = "Conflict";
        error_message = "The request could not be completed due to a conflict "
                        "with the current state of the resource.";
        break;
    case 504:
        status_code = 504;
        reason_phrase = "Gateway Timeout";
        error_message =
            "The server, while acting as a gateway or proxy, did not receive a "
            "timely response from the upstream server.";
        break;
    default:
        status_code = 500;
        reason_phrase = "Internal Server Error";
        error_message = "An internal server error occurred.";
        break;
    }
    std::string response_body =
        "<html>\n<head><title>" + toString(status_code) + " " + reason_phrase +
        "</title></head>\n<body>\n<h1>" + reason_phrase + "</h1><br><br>\n<p>" +
        error_message + "</p>\n</body>\n</html>\n";

    std::string response_headers =
        "HTTP/1.1 " + toString(status_code) + " " + reason_phrase +
        "\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: " +
        toString(response_body.length()) + "\r\nConnection: close\r\n\r\n";

    send(client_fd, response_headers.c_str(), response_headers.length(), 0);
    send(client_fd, response_body.c_str(), response_body.length(), 0);
}
