/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:34:18 by ebmarque          #+#    #+#             */
/*   Updated: 2024/10/06 11:57:39 by ebmarque         ###   ########.fr       */
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
#include "Parser.hpp"

#define MAXPATHLEN 4096
#define CGI_TIMEOUT 5

typedef struct pollfd t_pollfd;

class CgiRequestHandler
{
	public:
		CgiRequestHandler(const Request &request, int fd,  const Config &config, std::map<pid_t, t_client_process> *open_processes);
		~CgiRequestHandler();

		static bool _canProcess(const Request &request, const std::vector<Route> &routes);

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
		Config		_server_config;
		std::string _method;
		std::string _resource;
		std::string _decoded_resource;
		std::string _protocol;
		std::string _headers;
		std::string _body;
		// ============================================================================
		std::map<pid_t, t_client_process> *_open_processes;
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
bool isExtensionAllowed(const std::string &url, const std::vector<std::string> &cgi_extensions);
void sendHttpErrorResponse(int client_fd, int error_code, const std::map<int, std::string> &errors);
unsigned int convertHex(const std::string &nb);

#endif // CGIREQUESTHANDLER_HPP
