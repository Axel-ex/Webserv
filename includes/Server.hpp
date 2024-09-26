/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 11:58:53 by Axel              #+#    #+#             */
/*   Updated: 2024/09/26 15:08:22 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "UserSession.hpp"

#include <exception>
#include <netinet/in.h>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <string>

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
        static std::vector<t_pollfd> _fds;
        std::map<std::string, UserSession> _activeSessions;

        void _acceptIncomingConnections(void);
        void _serveClients(void);
		ssize_t _readFd(int fd, char *buffer, size_t buffer_size);
};

#endif // SERVER_HPP
