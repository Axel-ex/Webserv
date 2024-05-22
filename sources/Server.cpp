/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 10:05:43 by Axel              #+#    #+#             */
/*   Updated: 2024/05/18 14:06:42 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include <cstddef>
#include <string>

Server ::Server(std::string config_file)
{
    Config::parseFile(config_file);
    Log::setLoglevel(DEBUG);
    Log::clearScreen();
}

Server ::~Server()
{
    for (size_t i = 0; i < _fds.size(); i++)
        close(_fds[i].fd);
}

void Server ::init()
{
    /* Imagine several ports are provided. we need to create several socket fds
     * for each port. We will then need to bind() and listen() each of them and
     * store it in our _fds vector. Information for the config will come for the
     * Config singleton*/

    for (size_t i = 0; i < Config::getPorts().size(); i++)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            throw ServerError("Fail creating socket");
        /* OS define a TIME_WAIT to reuse socket to make sure that all TCP
         * packets are send before reusing the port/socket for another purpose.
         * setting this option enables us to launch our server again straight
         * after closing it*/
        int optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        t_sockaddr_in address;
        std::memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(Config::getPorts()[i]);

        if (bind(sockfd, (t_sockaddr*)&address, sizeof(address)) < 0)
            throw ServerError("Fail binding the socket");

        if (listen(sockfd, MAX_CLIENT) < 0)
            throw ServerError("fail listening for conncetion");

        /* socket fd is stored in the _fds to poll to detect incoming
         * connection*/
        t_pollfd new_fd;
        std::memset(&new_fd, 0, sizeof(new_fd));
        new_fd.fd = sockfd;
        new_fd.events = POLLIN;
        _fds.push_back(new_fd);
    }

    std::string port_info = "server listening on ports:";
    for (size_t i = 0; i < Config::getPorts().size(); i++)
        port_info += " " + std::to_string(Config::getPorts()[i]);
    Log::log(INFO, port_info);
}

void Server ::start(void)
{
    while (true)
    {
        /* Check for any change in our file descriptors */
        int activity = poll(_fds.data(), MAX_CLIENT, -1);
        if (activity < 0)
            throw ServerError("Error in poll");

        _acceptIncomingConnections();
        _serveClients();
    }
}

void Server ::_acceptIncomingConnections(void)
{
    /* listen for event on the server sockets*/
    for (size_t i = 0; i < Config::getPorts().size(); i++)
    {
        /* If we detect any events on the server socket, add an fd in the poll
         * list for client connections*/
        if ((_fds[i].revents & POLLIN))
        {
            int newfd = accept(_fds[i].fd, NULL, NULL);
            if (newfd < 0)
                throw ServerError("Error accepting connection");

            t_pollfd new_pollfd;
            std::memset(&new_pollfd, 0, sizeof(new_pollfd));
            new_pollfd.fd = newfd;
            new_pollfd.revents = POLLIN;
            _fds.push_back(new_pollfd);
        }
    }
}

void Server ::_serveClients(void)
{
    /*listen for client events, skip the first fds that are for the server sockets
     */
    for (size_t i = Config::getPorts().size(); i < _fds.size(); ++i)
    {
        if (_fds[i].revents & POLLIN)
        {
            char buffer[1024];

            std::memset(buffer, 0, sizeof(buffer));
            ssize_t n = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
            if (n < 0)
            {
                close(_fds[i].fd);
                _fds.erase(_fds.begin() + i);
                Log::log(ERROR, "reading client request");
                continue;
            }

            Request request(buffer);
            Response response(request);

            Log::log(DEBUG, buffer);
            send(_fds[i].fd, response.getHeaders().c_str(),
                 response.getHeaders().size(), 0);
            send(_fds[i].fd, response.getBody().c_str(),
                 response.getBody().size(), 0);
            close(_fds[i].fd);
            _fds.erase(_fds.begin() + i);
        }
    }
}
