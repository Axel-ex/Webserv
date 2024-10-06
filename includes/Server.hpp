/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 11:58:53 by Axel              #+#    #+#             */
/*   Updated: 2024/10/06 12:28:45 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include <exception>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <cstdio>

typedef struct pollfd t_pollfd;
typedef struct sockaddr_in t_sockaddr_in;
typedef struct sockaddr t_sockaddr;

#define MAX_CLIENT 500
#define SERVER_TIMEOUT 4

extern std::vector<t_chldProcess> finished_pids;

typedef struct _s_client_process
{
	std::string method;
	double start_time;
	int client_fd;
	int cgi_fd;
} t_client_process;

class Server
{
    public:
        static void _sigchldHandler(int signum);
        void _checkTimeouts();
        void checkFinishedProcesses(void);

        Server(Config& config);
        ~Server();

        std::vector<t_pollfd> init(void);
        void acceptIncomingConnections(t_pollfd &fd);
        void serveClients(void);

        Config& getConfig(void);

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
        std::vector<t_pollfd> _client_fds;
        std::map<pid_t, t_client_process> _open_processes;
        ssize_t _readFd(int fd, char* buffer, size_t buffer_size);
        void    finishCgiResponse(t_chldProcess child);
};

#endif // SERVER_HPP
