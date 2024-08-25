/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:34:18 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/25 15:24:50 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIREQUESTHANDLER_HPP
#define CGIREQUESTHANDLER_HPP

#include "Request.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "RequestHandlers.hpp"
#include "Response.hpp"
#include "utils.hpp"


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
#include <fcntl.h>
#include <cstdio>

#define MAXPATHLEN 4096
#define CGI_TIMEOUT 5

typedef struct pollfd t_pollfd;
typedef struct _s_client_process
{
	std::string				method;
	pid_t					pid;
	clock_t					start_time;
	int						client_fd;
	int						cgi_fd;
	t_pollfd				pollfd;
}				t_client_process;

class CgiRequestHandler
{
	public:
		CgiRequestHandler(const Request &request, int fd, Route &location, t_pollfd pollfd);
		~CgiRequestHandler();

		// static void _checkTimeouts();
		static bool _canProcess(const Request &request);
		// static void	_sigchldHandler(int signum);
		
		static std::map<pid_t, t_client_process> _open_processes;

		void processRequest();

	private:
	
		// ============================ REQUEST INFORMATION ===========================
		
		std::string							_method;
		std::string							_resource;
		std::string							_protocol;
		std::string							_headers;
		std::string							_body;
		// ============================================================================
		
		std::map<std::string, std::string>	_env;
		std::map<std::string, std::string>	_request_headers;		
		std::string							_scriptName;
		std::string							_scriptPath;
		std::string							_extension;
		std::string							_document_root;
		std::string							_query_str;

		t_pollfd							_pollfd;
		Route								_location;
		int									_client_fd;
		char								**_ch_env;
		char								*_argv[3];


		void 								initCgiEnv(void);
		void 								initChEnv(void);
		std::string							decode();
		std::string							getContentType(const std::string &headers);
		std::string							getScriptPath(void) const;
		std::string							getScriptName(void) const;
		std::string 						getWorkingPath(void) const;
		std::string							getQueryString(void);
		std::map<std::string, std::string>	parseHttpHeader(void) const;
};


std::string getFileExtension(const std::string &url);
std::string intToString(int value);
bool		startsWith(std::string str, const std::string &prefix);
bool 		isExtensionAllowed(const std::string &url, const std::vector<std::string> &cgi_extensions);
#endif // CGIREQUESTHANDLER_HPP
