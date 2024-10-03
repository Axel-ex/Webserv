/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 11:58:53 by Axel              #+#    #+#             */
/*   Updated: 2024/10/03 14:54:27 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <exception>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "Config.hpp"


typedef struct pollfd t_pollfd;
typedef struct sockaddr_in t_sockaddr_in;
typedef struct sockaddr t_sockaddr;

#define MAX_CLIENT 500
#define SERVER_TIMEOUT 4

class Server
{
    public:
        static void _sigchldHandler(int signum);
        void _checkTimeouts();

        Server(Config &config);
        ~Server();

        void init(void);
        void start(void);
		Config &getConfig(void);

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
		Config _config;
		//WARNING: why static?
        std::vector<t_pollfd> _fds;

        void _acceptIncomingConnections(void);
        void _serveClients(void);
        ssize_t _readFd(int fd, char* buffer, size_t buffer_size);
};

#endif // SERVER_HPP
