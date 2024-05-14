/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiple_cli_server.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 11:58:53 by Axel              #+#    #+#             */
/*   Updated: 2024/05/12 09:42:57 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CLIENT 5

typedef struct sockaddr_in sockaddr_in;

class Server
{
    public:
        Server(int port) : _port(port)
        {
            for (int i = 0; i < MAX_CLIENT + 1; i++)
                _fds[i].fd = -1;
        }
        ~Server() { close(_sockfd); }

        void init(void)
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
                throw ServerError("Fail creating socket");

            memset(&_address, 0, sizeof(_address));
            _address.sin_family = AF_INET;
            _address.sin_addr.s_addr = INADDR_ANY;
            _address.sin_port = htons(_port);

            if (bind(_sockfd, (struct sockaddr*)&_address, sizeof(_address)) <
                0)
                throw ServerError("Fail binding the socket");

            listen(_sockfd, MAX_CLIENT);
            /* server fd is stored in the fd to poll to detect incoming
             * cinnection*/
            _fds[0].fd = _sockfd;
            _fds[0].events = POLLIN;
        }

        void start(void)
        {
            std::cout << "Starting server on 127.0.0.1:" << _port << std::endl;

            while (true)
            {
                /* Check for incoming connections */
                int activity = poll(_fds, MAX_CLIENT + 1, -1);
                if (activity < 0)
                    throw ServerError("Error in poll");

                int newsock;
                /* If ther is accept it */
                if (_fds[0].revents & POLLIN)
                {
                    newsock = accept(_sockfd, NULL, NULL);
                    if (newsock < 0)
                    {
                        std::cerr << "Error accepting connection" << std::endl;
                        continue;
                    }
                    /* Find empty slot in the array */
                    int i;
                    for (i = 1; i < MAX_CLIENT + 1; i++)
                    {
                        if (_fds[i].fd == -1)
                        {
                            _fds[i].fd = newsock;
                            _fds[i].events = POLLIN;
                            break;
                        }
                    }
                    if (i == MAX_CLIENT + 1)
                        throw ServerError("Maximum connections reached.");
                }
				
				/* listen to the request of each clients */
                for (int i = 1; i < MAX_CLIENT + 1; ++i)
                {
                    if (_fds[i].fd != -1 && (_fds[i].revents & POLLIN))
                    {
                        char buffer[1024];
                        ssize_t n = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
                        if (n <= 0)
                        {
                            close(_fds[i].fd);
                            _fds[i].fd = -1;
                        }
                        else
                        {
                            std::cout << buffer << std::endl;
                            // Echo back to client
                            write(_fds[i].fd, buffer, n);
                        }
                    }
                }
            }
        }

        class ServerError : public std::exception
        {
            public:
                ServerError(const std::string& error) : msg(error) {}
                ~ServerError() throw() {}
                const char* what() const throw() { return (msg.c_str()); }

            private:
                std::string msg;
        };

    private:
        /* _sockfd will be an array of sockets */
        int _sockfd;
        int _port;
        sockaddr_in _address;
        struct pollfd _fds[MAX_CLIENT + 1]; // +1 for server socket
};

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Enter a port number to connect to." << std::endl;
        exit(1);
    }

    int port = std::atoi(argv[1]);
    Server server(port);

    try
    {
        server.init();
        server.start();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
