/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 10:05:43 by Axel              #+#    #+#             */
/*   Updated: 2024/10/12 12:52:17 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/CgiRequestHandler.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include "../includes/utils.hpp"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

Server::Server(Config& config) : _config(config) {}

Server::~Server()
{
    for (size_t i = 0; i < _client_fds.size(); i++)
    {
        close(_client_fds[i].fd);
    }
    std::string info_string =
        "Server " + _config.getServerName() + " shutting down";
    Log::log(INFO, info_string);
}

Config& Server::getConfig(void) { return _config; }

std::vector<t_pollfd> Server::init()
{
    std::vector<t_pollfd> poll_fds;

    for (size_t i = 0; i < _config.getPorts().size(); i++)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            throw ServerError("Fail creating socket");
        int optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        t_sockaddr_in address;
        std::memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(_config.getPorts()[i]);

        if (bind(sockfd, (t_sockaddr*)&address, sizeof(address)) < 0)
            throw ServerError("Fail binding the socket");

        if (listen(sockfd, MAX_CLIENT) < 0)
            throw ServerError("fail listening for connection");

        /* Create the poll_fd and add it to the vector*/
        t_pollfd new_fd;
        std::memset(&new_fd, 0, sizeof(new_fd));
        new_fd.fd = sockfd;
        new_fd.events = POLLIN;
        poll_fds.push_back(new_fd);
    }
    announce(false);

    return poll_fds;
}

void Server::announce(bool virtual_server)
{
    std::string port_info =
        "Server " + _config.getServerName() + " listening on ports:";
    for (size_t i = 0; i < _config.getPorts().size(); i++)
        port_info += " " + toString(_config.getPorts()[i]);
    if (virtual_server)
		port_info += " (virtual)";
    Log::log(INFO, port_info);
}

void Server::acceptIncomingConnections(t_pollfd& poll_fd)
{
    int newfd = accept(poll_fd.fd, NULL, NULL);
    if (newfd < 0 && !stopFlag)
        throw ServerError("Error accepting connection");

    t_pollfd new_client_fd;
    std::memset(&new_client_fd, 0, sizeof(new_client_fd));
    new_client_fd.fd = newfd;
    new_client_fd.revents = POLLIN;
    _client_fds.push_back(new_client_fd);
}

ssize_t Server::_readFd(int fd_index, char* buffer, size_t buffer_size)
{
    ssize_t n = recv(_client_fds[fd_index].fd, buffer, buffer_size, 0);
    if (n < 0)
    {
        Log::log(ERROR, "reading client request");
        std::cout << "from server: " << _config.getServerName() << std::endl;
    }
    return (n);
}

// ==========================================================================================
// 									LOOK FOR TIMED-OUT PROCESSES
// ==========================================================================================
void Server::checkTimeouts()
{
    long now;
    long elapsed;
    std::map<pid_t, t_client_process>::iterator it = _open_processes.begin();
    for (; it != _open_processes.end(); it++)
    {
        now = ServerTools::getTime();
        elapsed = now - it->second.start_time;
        Log::log(WARNING, ("Process [" + toString(RED) + toString(it->first) +
                           toString(RESET) + "]" +
                           " running for: " + toString(elapsed) + " seconds."));
        if (elapsed > CGI_TIMEOUT)
        {
            Log::log(DEBUG,
                     ("CGI PROCESS [" + toString(RED) + toString(it->first) +
                      RESET + "]: Exceeded the time limit."));
            CgiTools::sendHttpErrorResponse(it->second.client_fd, ETIMEDOUT,
                                            _config.getErrors());
            kill(it->first, SIGKILL);
        }
    }
}

void Server::_finishCgiResponse(t_chldProcess child)
{
    t_client_process client = _open_processes[child.pid];

    if (child.type == EXITED)
    {
        if (WEXITSTATUS(child.status) == 0)
        {
            Log::log(DEBUG,
                     ("CGI PROCESS [" + toString(RED) + toString(child.pid) +
                      RESET + "] HAS FINISHED ITS EXECUTION."));
            char buffer[BUFSIZ];
            std::string response;
            std::string ok = "HTTP/1.1 200 OK\r\n";
            ssize_t bytesRead;
            while ((bytesRead = read(client.cgi_fd, buffer, BUFSIZ)) > 0)
                response += std::string(buffer, bytesRead);
            send(client.client_fd, ok.c_str(), ok.length(), 0);
            send(client.client_fd, response.c_str(), response.length(), 0);
        }
        else
        {
            Log::log(ERROR,
                     ("CGI PROCESS [" + toString(RED) + toString(child.pid) +
                      RESET + "] FINISHED WITH EXIT CODE: " +
                      toString(WEXITSTATUS(child.status))));
            CgiTools::sendHttpErrorResponse(client.client_fd, 500,
                                            std::map<int, std::string>());
        }
    }
    else
    {
        if (WTERMSIG(child.status) == SIGKILL)
            Log::log(DEBUG,
                     ("CGI PROCESS [" + toString(RED) + toString(child.pid) +
                      RESET + "] HAS BEEN KILLED."));
        else
        {
            Log::log(
                ERROR,
                ("CGI PROCESS [" + toString(RED) + toString(child.pid) + RESET +
                 "] RECEIVED THE SIGNAL: " + toString(WTERMSIG(child.status))));
            CgiTools::sendHttpErrorResponse(client.client_fd, 500,
                                            std::map<int, std::string>());
        }
    }
    _fds_to_close.push_back(client.client_fd);
    close(client.cgi_fd);
    _open_processes.erase(child.pid);
}

void Server::checkFinishedProcesses(void)
{
    std::deque<t_chldProcess>::iterator it = finished_pids.begin();
    for (; it != finished_pids.end();)
    {
        if (_open_processes.find(it->pid) != _open_processes.end())
        {
            _finishCgiResponse(*it);
            it = finished_pids.erase(it);
        }
        else
            it++;
    }
    closePendingFds();
}

void Server::serveClients(void)
{
    for (size_t i = 0; i < _client_fds.size(); ++i)
    {
        if (_client_fds[i].revents & POLLIN)
        {
            char read_buffer[1024];
            RequestBuffer request_buffer;
            clock_t start = ServerTools::getTime();

            // Read from client chunks. set a TIMEOUT for long request.
            while (!request_buffer.isRequestOver())
            {
                clock_t curr = ServerTools::getTime();
                double elapsed = curr - start;
                if (elapsed > SERVER_TIMEOUT)
                    break;

                std::memset(read_buffer, 0, sizeof(read_buffer));
                ssize_t n = _readFd(i, read_buffer, sizeof(read_buffer));
				if (n < 0)
				{
					request_buffer.getBuffer().clear();
					break;
				}
                request_buffer.appendBuffer(read_buffer, n);
            }
            if (request_buffer.getBuffer().empty())
            {
				_fds_to_close.push_back(_client_fds[i].fd);
                continue;
            }

            Request request(request_buffer.getBuffer());
            sendResponse(request, i);
        }
    }
}

/**
 * @brief Match the request hostname to respond to the client with the right
 * server config
 *
 * @param request
 * @param fd_index
 */
void Server::sendResponse(Request& request, int fd_index)
{
    Config config = _matchHostConfig(request);

    if (CgiRequestHandler::_canProcess(request, config.getRoutes()))
    {
        CgiRequestHandler cgi_obj(request, _client_fds[fd_index].fd, config,
                                  &_open_processes);
        cgi_obj.processRequest();
    }
    else
    {
        Response response(request, config);
        send(_client_fds[fd_index].fd, response.getResponseBuffer().c_str(),
             response.getResponseBuffer().size(), 0);
        _fds_to_close.push_back(_client_fds[fd_index].fd);
    }
    Log::logRequest(request, config.getServerName());
}

/**
 * @brief check if the request host is one of the virtual server and return the
 * right config to answer the client.
 *
 * @param request
 * @return
 */
const Config& Server::_matchHostConfig(Request& request)
{
    std::string hostname = request.getHost();
    std::vector<Server*>::iterator it_virtual_servers =
        _virtual_servers.begin();

    for (; it_virtual_servers != _virtual_servers.end(); it_virtual_servers++)
    {
        Config& config = (*it_virtual_servers)->getConfig();
        if (hostname.find(config.getServerName()) != std::string::npos)
            return (config);
    }
    return (_config);
}

void Server::closePendingFds(void)
{
    for (std::deque<int>::iterator it_fd = _fds_to_close.begin();
         it_fd != _fds_to_close.end();)
    {
        // Find and close the FD in _client_fds using the functor
        std::deque<t_pollfd>::iterator client_it =
            std::find_if(_client_fds.begin(), _client_fds.end(),
                         ServerTools::MatchFd(*it_fd));

        if (client_it != _client_fds.end())
        {
            close(client_it->fd);
            _client_fds.erase(client_it);
        }
        it_fd = _fds_to_close.erase(it_fd);
    }
}

void Server::addVirtualServer(Server* server)
{
    _virtual_servers.push_back(server);
}
