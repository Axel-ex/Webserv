/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Single_cli_server.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 08:46:52 by Axel              #+#    #+#             */
/*   Updated: 2024/05/12 07:51:15 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

int error(const std::string& msg)
{
    std::cerr << "error: " << msg << std::endl;
    exit(EXIT_FAILURE);
}

class Server
{
    public:
        Server(){};
        ~Server()
        {
            close(_sockfd);
            close(_clifd);
        };

        void init(std::string portnb)
        {
            _portnb = std::atoi(portnb.c_str());
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
                error("Creating the socket");

            /* Init server address */
            /* IPV4 ffamilly*/
            _serv_addr.sin_family = AF_INET;
            _serv_addr.sin_addr.s_addr = INADDR_ANY;
            /* htons -> host to network short, transform the port into
             * appropriate bit format */
            _serv_addr.sin_port = htons(_portnb);

            /* bind file descriptor with address*/
            if (bind(_sockfd, (struct sockaddr*)&_serv_addr,
                     sizeof(_serv_addr)) < 0)
                error("Binding socket");
        };

        void listen(void)
        {
            /* listen for connection (max set to 5)*/
            ::listen(_sockfd, 5);
            std::cout << "listening on port " << _portnb << std::endl;

            /*Create client */
            struct sockaddr_in cli_addr;
            socklen_t clilen = sizeof(cli_addr);

            _clifd = accept(_sockfd, (struct sockaddr*)&cli_addr, &clilen);
            if (_clifd < 0)
                error("Accepting client");

            while (true)
            {
                int n;

                bzero(buff, 255);
                n = recv(_clifd, buff, 255, MSG_WAITALL);
                if (n < 0)
                    error("On reading");
                printf("request received: %s\n", buff);
            }
        };

    private:
        int _sockfd;
        int _clifd; /* This will have to be an array. this is the array we
                           will poll on to see if we have incoming requests*/
        int _portnb;
        char buff[255];
        struct sockaddr_in _serv_addr;
};

int main(int argc, char** argv)
{
    if (argc != 2)
        return (error("invalid number of arguments input"));

    Server serv;

    serv.init(argv[1]);
    serv.listen();

    return (EXIT_SUCCESS);
}
