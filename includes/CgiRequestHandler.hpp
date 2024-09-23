/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:34:18 by ebmarque          #+#    #+#             */
/*   Updated: 2024/09/23 22:07:14 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIREQUESTHANDLER_HPP
#define CGIREQUESTHANDLER_HPP

#include "Request.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "RequestHandlers.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include <cerrno>

#define MAXPATHLEN 4096
#define CGI_TIMEOUT 5

typedef struct pollfd t_pollfd;
typedef struct _s_client_process
{
	std::string method;
	double start_time;
	int client_fd;
	int cgi_fd;
} t_client_process;

class CgiRequestHandler
{
	public:
		CgiRequestHandler(const Request &request, int fd, Route &location);
		~CgiRequestHandler();

		static bool _canProcess(const Request &request);
		static std::map<pid_t, t_client_process> _open_processes;

		void processRequest();

		class CgiError : public std::exception
		{
		public:
			CgiError(const std::string &error) : msg(error) {}
			~CgiError() throw() {}
			const char *what() const throw() { return (msg.c_str()); }

		private:
			std::string msg;
		};

	private:
		// ============================ REQUEST INFORMATION ===========================

		std::string _method;
		std::string _resource;
		std::string _decoded_resource;
		std::string _protocol;
		std::string _headers;
		std::string _body;
		// ============================================================================

		std::map<std::string, std::string> _env;
		std::map<std::string, std::string> _request_headers;
		std::string _scriptName;
		std::string _scriptPath;
		std::string _pathInfo;
		std::string _pathTranslated;
		std::string _extension;
		std::string _interpreter;
		std::string _document_root;
		std::string	_query_str;
		Route		_location;

		int 		_client_fd;
		int			_pipe_in[2];
		int			_pipe_out[2];
		char 		**_ch_env;
		char 		*_argv[3];
		
		void 								initCgiEnv(void);
		void 								initChEnv(void);
		void 								getPathInfo(void);
		std::string 						decode();
		std::string 						getRootPath(void) const;
		std::string 						getInterpreter(void) const;
		std::string 						getContentType(const std::string &headers);
		std::string 						getScriptPath(void) const;
		std::string 						getScriptName(void) const;
		std::string 						getWorkingPath(void) const;
		std::string 						getQueryString(void);
		std::map<std::string, std::string>	parseHttpHeader(void) const;
};

std::string getFileExtension(const std::string &url);
std::string intToString(int value);
bool startsWith(std::string str, const std::string &prefix);
bool isExtensionAllowed(const std::string &url, const std::vector<std::string> &cgi_extensions);
void sendHttpErrorResponse(int client_fd, int error_code);
unsigned int convertHex(const std::string &nb);

#endif // CGIREQUESTHANDLER_HPP
