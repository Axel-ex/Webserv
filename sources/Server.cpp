/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 10:05:43 by Axel              #+#    #+#             */
/*   Updated: 2024/09/05 10:37:48 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include "../includes/utils.hpp"
#include "../includes/CgiRequestHandler.hpp"
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <cerrno>

std::vector<t_pollfd> Server::_fds;

Server::Server(std::string config_file)
{
	Parser parser;

	parser.parse(config_file);
	Log::setLogLevel(DEBUG);
	Log::clearScreen();
}

Server::~Server()
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		close(_fds[i].fd);
	}
	Log::log(INFO, "Server shutting down");
}

void Server::init()
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

		if (bind(sockfd, (t_sockaddr *)&address, sizeof(address)) < 0)
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
		port_info += " " + toString(Config::getPorts()[i]);
	Log::log(INFO, port_info);
}

void Server::start(void)
{
	while (!stopFlag)
	{
		int activity;

		// Loop to handle EINTR (syscall interruptions while polling) error
		do {
			// Check for any change in our file descriptors
			activity = poll(_fds.data(), _fds.size(), 1000);
		} while (activity < 0 && errno == EINTR);
		
		if (activity < 0 && !stopFlag) {          
			perror("poll");
			throw ServerError("Error in poll");
		}
		_checkTimeouts();
		_acceptIncomingConnections();
		_serveClients();
	}
}

void Server::_acceptIncomingConnections(void)
{
	/* listen for event on the server sockets*/
	for (size_t i = 0; i < Config::getPorts().size(); i++)
	{
		/* If we detect any events on the server socket, add an fd in the poll
		 * list for client connections*/
		if ((_fds[i].revents & POLLIN))
		{
			int newfd = accept(_fds[i].fd, NULL, NULL);
			if (newfd < 0 && !stopFlag)
				throw ServerError("Error accepting connection");

			t_pollfd new_pollfd;
			std::memset(&new_pollfd, 0, sizeof(new_pollfd));
			new_pollfd.fd = newfd;
			new_pollfd.revents = POLLIN;
			_fds.push_back(new_pollfd);
		}
	}
}

ssize_t Server::_readFd(int fd_index, char *buffer, size_t buffer_size)
{
	ssize_t n = recv(_fds[fd_index].fd, buffer, buffer_size, 0);
	if (n < 0)
	{
		close(_fds[fd_index].fd);
		_fds.erase(_fds.begin() + fd_index);
		Log::log(ERROR, "reading client request");
	}
	return (n);
}

Route getCgiRoute(const Request &request)
{
	std::vector<Route> routes = Config::getRoutes();
	std::string resource = request.getResource();
	std::string method = request.getMethod();

	for (size_t i = 0; i < routes.size(); i++)
	{
		if (std::find(routes[i].methods.begin(), routes[i].methods.end(), method) != routes[i].methods.end())
		{
			if (startsWith(resource, routes[i].url) && isExtensionAllowed(resource, routes[i].cgi_extension))
				return (routes[i]);
		}
	}
	return (routes[0]);
}

// ==========================================================================================
// 									LOOK FOR TIMED-OUT PROCESSES
// ==========================================================================================
void Server::_checkTimeouts()
{
	clock_t now;
	double elapsed;
	std::map<pid_t, t_client_process>::iterator it = CgiRequestHandler::_open_processes.begin();
	for (; it != CgiRequestHandler::_open_processes.end(); it++)
	{
		now = clock();
		elapsed = static_cast<double>(now - it->second.start_time) / CLOCKS_PER_SEC * 1e4;
		Log::log(WARNING, ("Process [" + toString(RED) + toString(it->first) + toString(RESET) + "]" + " running for: " + toString(elapsed) + " seconds."));
		if (elapsed > CGI_TIMEOUT)
		{
			Log::log(DEBUG, ("CGI PROCESS [" + toString(RED) + toString(it->first) + RESET + "]: Exceeded the time limit."));
			sendHttpErrorResponse(it->second.client_fd, ETIMEDOUT);
			kill(it->first, SIGKILL);
		}
	}
}

// ==========================================================================================
// 						SIGNAL HANDLER FOR FINISHED/INTERRUPTED PROCESSES
// ==========================================================================================
void Server::_sigchldHandler(int signum)
{
	(void)signum;
	int status;
	pid_t pid;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		std::map<pid_t, t_client_process>::iterator it = CgiRequestHandler::_open_processes.find(pid);
		int fd_position = 0;
		int client_fd = it->second.client_fd;
		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].fd == it->second.client_fd)
			{
				fd_position = i;
				break;
			}
		}
		if (WIFEXITED(status))
		{
			if (WEXITSTATUS(status) == 0)
			{
				Log::log(DEBUG, ("CGI PROCESS [" + toString(RED) + toString(pid) + RESET + "] HAS FINISHED ITS EXECUTION."));
				char buffer[BUFSIZ];
				std::string response;
				ssize_t bytesRead;
				while ((bytesRead = read(it->second.cgi_fd, buffer, BUFSIZ)) > 0)
					response += std::string(buffer, bytesRead);
				send(it->second.client_fd, response.c_str(), response.length(), 0);
			}

		}
		else if (WIFSIGNALED(status))
		{
			int signal = WTERMSIG(status);
			if (signal == SIGKILL)
				Log::log(DEBUG, ("CGI PROCESS [" + toString(RED) + toString(pid) + RESET + "] HAS BEEN KILLED."));
			else
				Log::log(DEBUG, ("CGI PROCESS [" + toString(RED) + toString(pid) + RESET + "RECEIVED THE SIGNAL: " + toString(signal)));
		}
		close(client_fd);
		close(it->second.cgi_fd);
		_fds.erase(_fds.begin() + fd_position);
		CgiRequestHandler::_open_processes.erase(it);
	}
}

void Server::_serveClients(void)
{
	/*listen for client events, skip the first fds that are for the server
	 * sockets */
	for (size_t i = Config::getPorts().size(); i < _fds.size(); ++i)
	{
		if (_fds[i].revents & POLLIN)
		{
			char read_buffer[1024];
			RequestBuffer request_buffer;
			clock_t start = clock();

			// Read from client chunks. set a TIMEOUT for long request.
			while (!request_buffer.isRequestOver())
			{
				clock_t curr = clock();
				double elapsed =
					static_cast<double>(curr - start) / CLOCKS_PER_SEC;
				if (elapsed > SERVER_TIMEOUT)
					break;

				std::memset(read_buffer, 0, sizeof(read_buffer));
				ssize_t n = _readFd(i, read_buffer, sizeof(read_buffer));
				if (n < 0)
					continue;
				request_buffer.appendBuffer(read_buffer, n);
			}

			/**
			 * Mozilla uses autocompletion and prefetching. sometimes it will
			 * send an empty request before the user even pressed enter to send
			 * the request
			 */
			if (request_buffer.getBuffer().empty())
			{
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				continue;
			}

			Request request(request_buffer.getBuffer());
			Log::logRequest(request);
			Log::log(DEBUG, request_buffer.getBuffer());

			if (CgiRequestHandler::_canProcess(request))
			{
				Route cgi_route = getCgiRoute(request);
				CgiRequestHandler cgi_obj(request, _fds[i].fd, cgi_route);
				cgi_obj.processRequest();
			}
			else
			{
				Response response(request);
				send(_fds[i].fd, response.getHeaders().c_str(),
					 response.getHeaders().size(), 0);
				send(_fds[i].fd, response.getBody().c_str(),
					 response.getBody().size(), 0);
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
			}
		}
	}
}
