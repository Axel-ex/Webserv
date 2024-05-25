/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 11:58:53 by Axel              #+#    #+#             */
/*   Updated: 2024/05/25 14:09:26 by achabrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <exception>
#include <netinet/in.h>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string>

typedef struct pollfd t_pollfd;
typedef struct sockaddr_in t_sockaddr_in;
typedef struct sockaddr t_sockaddr;

#define MAX_CLIENT 50

class Server
{
    public:
        Server(std::string config_file);
        ~Server();

        void init(void);
        void start(void);

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
        std::vector<t_pollfd> _fds;
	
        void _acceptIncomingConnections(void);
        void _serveClients(void);
		ssize_t _readFd(int fd, char *buffer, size_t buffer_size);
};

#endif // SERVER_HPP
