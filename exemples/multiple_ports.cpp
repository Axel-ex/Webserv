/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiple_cli3.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 11:58:53 by Axel              #+#    #+#             */
/*   Updated: 2024/05/13 11:59:29 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define MAX_CLIENT 5

typedef struct sockaddr_in sockaddr_in;

class Server
{
    public:
        Server(int port)
        {
            _port[0] = port;
            _port[1] = 9000;
        }
        ~Server()
        {
		
        }

        void init(void)
        {
            for (int i = 0; i < 2; i++)
            {
				int sockfd;

                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0)
                    throw ServerError("Fail creating socket");

                memset(&_address, 0, sizeof(_address));
                _address.sin_family = AF_INET;
                _address.sin_addr.s_addr = INADDR_ANY;
                _address.sin_port = htons(_port[i]);

                if (bind(sockfd, (struct sockaddr*)&_address,
                         sizeof(_address)) < 0)
                    throw ServerError("Fail binding the socket");

                listen(sockfd, MAX_CLIENT);
                /* server fd is stored in the fd to poll to detect incoming
                 * connection*/
				struct pollfd new_fd;

                new_fd.fd = sockfd;
                new_fd.events = POLLIN;
				_fds.push_back(new_fd);
            }
        }

        void start(void)
        {
            std::cout << "Starting server on 127.0.0.1:" << _port << std::endl;

            while (true)
            {
                /* Check for incoming connections */
                int activity = poll(_fds.data(), MAX_CLIENT, -1);
                if (activity < 0)
                    throw ServerError("Error in poll");


                for (int i = 0; i < 2; i++)
                {
                    if ((_fds[i].revents & POLLIN))
                    {
                        int newfd = accept(_fds[i].fd, NULL, NULL);
                        if (newfd < 0)
                        {
                            std::cerr << "Error accepting connection"
                                      << std::endl;
                            continue;
                        }

						struct pollfd new_pollfd;

						memset(&new_pollfd, 0, sizeof(new_pollfd));
						new_pollfd.fd = newfd;
						new_pollfd.revents = POLLIN;
						_fds.push_back(new_pollfd);
                    }
                }

                /* listen to the request of each clients */
                for (int i = 2; i < _fds.size(); ++i)
                {
                    if (_fds[i].fd != -1 && (_fds[i].revents & POLLIN))
                    {
                        char buffer[1024];
						
						bzero(buffer, sizeof(buffer));
                        ssize_t n = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
                        if (n < 0)
                        {
                            close(_fds[i].fd);
                            _fds[i].fd = -1;
                            std::cout << "ERROR" << std::endl;
                        }
                        else
                        {
                            std::cout << buffer << std::endl;
                            /* close(_fds[i].fd); */
                            /* _fds[i].fd = -1; */
                            // Echo back to client
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
        int _port[2];
        sockaddr_in _address;
		std::vector<struct pollfd> _fds;
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
