/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 10:05:43 by Axel              #+#    #+#             */
/*   Updated: 2024/05/16 13:37:09 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Config.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include <cstddef>
#include <iostream>

Server ::Server(std::string config_file)
{
	Config::parseFile(config_file);
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

        sockaddr_in address;
        std::memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(Config::getPorts()[i]);

        if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0)
            throw ServerError("Fail binding the socket");

        if (listen(sockfd, MAX_CLIENT) < 0)
            throw ServerError("fail listening for conncetion");

        /* socket fd is stored in the _fds to poll to detect incoming
         * connection*/
        struct pollfd new_fd;
        std::memset(&new_fd, 0, sizeof(new_fd));
        new_fd.fd = sockfd;
        new_fd.events = POLLIN;
        _fds.push_back(new_fd);
    }

    std::cout << "server listening on ports ";
    for (size_t i = 0; i < Config::getPorts().size(); i++)
        std::cout << Config::getPorts()[i] << " ";
    std::cout << std::endl;
}

void Server ::start(void)
{
    while (true)
    {
        /* Check for incoming connections */
        int activity = poll(_fds.data(), MAX_CLIENT, -1);
        if (activity < 0)
            throw ServerError("Error in poll");

        _acceptIncomingConnections();
        _serveClients();
    }
}

void Server ::_acceptIncomingConnections(void)
{
    /* listen for event on the server socket*/
    for (size_t i = 0; i < Config::getPorts().size(); i++)
    {
        /* If we detect an events on the server socket, add an fd in the poll
         * list for client connection*/
        if ((_fds[i].revents & POLLIN))
        {
            int newfd = accept(_fds[i].fd, NULL, NULL);
            if (newfd < 0)
                throw ServerError("Error accepting connection");

            struct pollfd new_pollfd;
            std::memset(&new_pollfd, 0, sizeof(new_pollfd));
            new_pollfd.fd = newfd;
            new_pollfd.revents = POLLIN;
            _fds.push_back(new_pollfd);
        }
    }
}

void Server ::_serveClients(void)
{
    /*listen for client events, skip the first fd that are for the server socket
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
                std::cerr << "error: reading client request" << std::endl;
                continue;
            }

		
			Request request(buffer);

            // TODO: build the response according to the request. the
            // response will take a request in its constructor
            // Response response(request);
            Response response;

            /* Print the request*/
            std::cout << buffer << std::endl;

			/* Send the response as first headers and body*/
            send(_fds[i].fd, response.get_headers().c_str(),
                 response.get_headers().size(), 0);
            send(_fds[i].fd, response.get_body(), strlen(response.get_body()),
                 0);

            /* Close connection */
            close(_fds[i].fd);
            _fds.erase(_fds.begin() + i);
        }
    }
}
