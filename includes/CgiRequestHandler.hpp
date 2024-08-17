/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:34:18 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/17 17:21:58 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIREQUESTHANDLER_HPP
#define CGIREQUESTHANDLER_HPP

#include "Request.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "RequestHandlers.hpp"
#include "Response.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>
#include <unistd.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <ctime>

#define MAXPATHLEN 4096

using std::map;
using std::string;

typedef struct _s_client_process
{
	pid_t	pid;
	int		client_fd;
	clock_t	start_time;
}				t_client_process;

class CgiRequestHandler
{
	public:
		CgiRequestHandler(const Request &request, int fd);
		CgiRequestHandler(const CgiRequestHandler &other);
		CgiRequestHandler &operator=(const CgiRequestHandler &other);
		~CgiRequestHandler();

		static bool _canProcess(const Request &request);
		static map<pid_t, t_client_process> _open_processes;

		void processRequest();
		void init_cgi_env(const Request &request);
		void init_ch_env();

	private:
		map<string, string>	_env;
		string				_method;
		string				_resource;
		string				_protocol;
		string				_headers;
		string				_body;
		Route				_location;
/* 		int					_in_pipe[2];
		int					_out_pipe[2]; */
		int					_client_fd;
		char				**_ch_env;
		char				*_argv[3];
};

#endif // CGIREQUESTHANDLER_HPP
