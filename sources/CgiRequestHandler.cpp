/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:36:42 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/22 21:40:11 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiRequestHandler.hpp"

// STRUCTURE TO BE USED FOR VERIFYING TIMED-OUT PROCESSES
std::map<pid_t, t_client_process> CgiRequestHandler::_open_processes;




CgiRequestHandler::CgiRequestHandler(const Request &request, int fd)
{
	// ============ REQUEST INFORMATION ==============

	_method = request.getMethod();
	_resource = request.getResource();
	_protocol = request.getProtocol();
	_headers = request.getHeaders();
	_body = request.getBody();
	// ===============================================

	_client_fd = fd;
	_scriptName = getScriptName();
	_scriptPath = getScriptPath();
	_request_headers = parseHttpHeader();
	_extension = getFileExtension(_resource);

	// ============ ENVIRONMENT INITIALIZATION ==============

	initCgiEnv();
	initChEnv();
	// ======================================================
}

CgiRequestHandler::~CgiRequestHandler()
{
	// ----> TODO: CODE THE DESTRUCTOR AND DEALOCATE MEMORY <----
}


/**
 * Determines whether the given request can be processed by the CGI request handler.
 *
 * @param request The request to be processed.
 * @return True if the request can be processed, false otherwise.
 */
bool CgiRequestHandler::_canProcess(const Request &request)
{
	std::vector<Route> routes = Config::getRoutes();
	std::string resource = request.getResource();
	// ===============================================================================

	// THE PARSING OF THE LOCATION IS FAILLING TO SAVE THE URL OF THE LOCATION
	// THE LOCATION URL IS NEEDED TO VERIFY IF THERE ARE ANY CGI DIRECTORIES DEFINED

	// ===============================================================================

	for (size_t i = 0; i < routes.size(); i++)
	{
		// NOT SURE IF WE WILL NEED TO HAVE THE CGI_PATH AS A REQUIREMENT !!!
		if (std::find(routes[i].methods.begin(), routes[i].methods.end(),
					  request.getMethod()) == routes[i].methods.end() ||
			routes[i].cgi_extension.empty() || routes[i].cgi_path.empty())
			continue;
		if (startsWith(resource, routes[i].url) && isExtensionAllowed(resource, routes[i].cgi_extension))
			return (true);
	}
	return (true);
}

void CgiRequestHandler::initCgiEnv()
{
	if (_method == "POST")
	{
		std::stringstream out;
		out << _body.length();
		_env["CONTENT_LENGTH"] = out.str();
		_env["CONTENT_TYPE"] = getContentType(_headers);
	}
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	// ===================================================================
	// 							FIX THIS DATA
	_env["SCRIPT_NAME"] = _scriptName;
	_env["SCRIPT_FILENAME"] = _scriptPath;
	_env["PATH_INFO"] = _scriptPath;
	_env["PATH_TRANSLATED"] = _scriptPath;
	// ===================================================================

	_env["REQUEST_URI"] = _resource;
	_env["SERVER_NAME"] = Config::getServerName();
	_env["SERVER_PORT"] = intToString(Config::getPorts()[0]);
	_env["REQUEST_METHOD"] = _method;
	_env["SERVER_PROTOCOL"] = _protocol;
	_env["REDIRECT_STATUS"] = "200";
	_env["SERVER_SOFTWARE"] = "Aether_42";
	_env["DOCUMENT_ROOT"] = _location.url;

	if (_request_headers.find("cookie") != _request_headers.end())
		_env["HTTP_COOKIE"] = _request_headers["cookie"];

	for (std::map<std::string, std::string>::iterator it = _request_headers.begin();
		 it != _request_headers.end(); ++it)
	{
		std::string name = it->first;
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		std::string key = "HTTP_" + name;
		_env[key] = it->second;
	}
	_argv[0] = strdup(_env["SCRIPT_FILENAME"].c_str());
	_argv[1] = strdup(_env["SCRIPT_FILENAME"].c_str());
	_argv[2] = NULL;
}

void CgiRequestHandler::initChEnv(void)
{
	int i = 0;
	std::map<std::string, std::string>::iterator it = _env.begin();

	_ch_env = new char *[_env.size() + 1];
	if (!_ch_env)
	{
		Log::log(ERROR, "Unable to allocate memory for CGI enviroment variables.");
		// exit(EXIT_FAILURE); look for cleaning function!!!
	}

	for (; it != _env.end(); it++, i++)
	{
		std::string variable = it->first + "=" + it->second;
		_ch_env[i] = strdup(variable.c_str());
	}
	_ch_env[i] = NULL;
}

void CgiRequestHandler::processRequest()
{
	pid_t pid;
	int cgi_pipe[2];

	if (pipe(cgi_pipe) < 0)
	{
		Log::log(ERROR, "Pipe function error in CGI process atempt.");
		// exit(EXIT_FAILURE); ---> SERVER SHUTDOWN POINT???
	}
	if ((pid = fork()) < 0)
	{
		Log::log(ERROR, "Fork function error in CGI process atempt.");
		// exit(EXIT_FAILURE); ---> SERVER SHUTDOWN POINT???
	}
	if (pid == 0)
	{
		close(cgi_pipe[0]);
		dup2(cgi_pipe[1], STDOUT_FILENO);
		close(cgi_pipe[1]);

		if (execve(_env["SCRIPT_FILENAME"].c_str(), _argv, _ch_env) < 0)
		{
			std::string failureResponse = "HTTP/1.1 500 Internal Server Error\r\n"
										  "Content-Type: text/plain\r\n\r\n"
										  "Content-Length: 21\r\n\r\n"
										  "Internal Server Error";
			send(_client_fd, failureResponse.c_str(), failureResponse.length(), 0);

			// exit(EXIT_FAILURE); SHOULD IT BE A SHUTDOWN POINT ???
		}
	}
	else
	{
		close(cgi_pipe[1]);
		fcntl(cgi_pipe[0], F_SETFL, O_NONBLOCK);

		t_client_process c_process = {_method, pid, clock(), _client_fd, cgi_pipe[0]};
		CgiRequestHandler::_open_processes[pid] = c_process;
	}
}

// ==========================================================================================
// 									LOOK FOR TIMED-OUT PROCESSES
// ==========================================================================================
void CgiRequestHandler::_checkTimeouts()
{
	clock_t now;
	double elapsed;
	for (std::map<pid_t, t_client_process>::iterator it = _open_processes.begin(); it != _open_processes.end(); it++)
	{
		now = clock();
		elapsed = static_cast<double>(now - it->second.start_time) / CLOCKS_PER_SEC * 1e4;
		if (elapsed > CGI_TIMEOUT)
		{
			kill(it->first, SIGKILL);
			close(it->second.cgi_fd);
			std::string failureResponse = "HTTP/1.1 500 Internal Server Error\r\n"
										  "Content-Type: text/plain\r\n\r\n"
										  "Content-Length: 21\r\n\r\n"
										  "Internal Server Error: Request timed-out.";
			send(it->second.client_fd, failureResponse.c_str(), failureResponse.length(), 0);
			close(it->second.client_fd);
			_open_processes.erase(it);
		}
	}
}


// ==========================================================================================
// 						SIGNAL HANDLER FOR FINISHED/INTERRUPTED PROCESSES
// ==========================================================================================
void CgiRequestHandler::_sigchldHandler(int signum)
{
	int status;
	pid_t pid;
	(void)signum;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		std::map<pid_t, t_client_process>::iterator it = _open_processes.find(pid);
		if (it != _open_processes.end())
		{
			int client_fd = it->second.client_fd;

			if (WIFEXITED(status))
			{
				char buffer[BUFSIZ];
				std::string response;
				ssize_t bytesRead;
				while ((bytesRead = read(it->second.cgi_fd, buffer, BUFSIZ)) > 0)
					response += std::string(buffer, bytesRead);

				send(it->second.client_fd, response.c_str(), response.length(), 0);
			}
			close(client_fd);
			close(it->second.cgi_fd);
			_open_processes.erase(it);
			std::cout << RED << "closed poll fd: " << client_fd << std::endl;
		}
		else
			std::cout << RED << "CGI PROCESS (" << pid <<  ") WAS KILLED" << std::endl;
		// Log::log(WARNING, "CGI PROCESS KILLED BY TIMEDOUT");
	}
}

// =========================================================================================
//									AUXILIAR FUNCTIONS
// =========================================================================================

std::string getFileExtension(const std::string &url)
{
	size_t queryPos = url.find('?');

	std::string path = (queryPos != std::string::npos) ? url.substr(0, queryPos) : url;

	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos)
		return ("");

	if (dotPos == path.size() - 1)
		return ("");

	return path.substr(dotPos + 1);
}

bool startsWith(const std::string &str, const std::string &prefix)
{
	if (prefix.size() > str.size())
		return (false);
	for (size_t i = 0; i < prefix.size(); ++i)
	{
		if (str[i] != prefix[i])
			return (false);
	}
	return (true);
}

bool isExtensionAllowed(const std::string &url, const std::vector<std::string> &cgi_extensions)
{
	std::string extension = getFileExtension(url);

	for (size_t i = 0; i < cgi_extensions.size(); ++i)
	{
		if (extension == cgi_extensions[i])
			return (true);
	}

	return (false);
}

std::string CgiRequestHandler::getWorkingPath(void) const
{
	char temp[MAXPATHLEN];
	return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

std::string CgiRequestHandler::getScriptPath(void) const
{
	// CORRECT THE RESOURCES LOCAITONS STATICALY NAMED
	std::string script = "/resources" + _resource;
	std::string wd = getWorkingPath();

	script = script.substr(0, script.find('?'));
	// if (script[0] != '/')
	return (wd + script);
	// return (script);
}

std::string CgiRequestHandler::getContentType(const std::string &headers)
{
	std::string content_type_key = "Content-Type: ";
	size_t content_type_pos = headers.find(content_type_key);
	if (content_type_pos == std::string::npos)
		return ("");
	content_type_pos += content_type_key.size();
	size_t content_type_end_pos;
	if (headers.find("\r\n", content_type_pos) < headers.find(";", content_type_pos))
		content_type_end_pos = headers.find("\r\n", content_type_pos);
	else
		content_type_end_pos = headers.find(";", content_type_pos);
	std::string content_type =
		headers.substr(content_type_pos, content_type_end_pos - content_type_pos);
	return (content_type);
}

std::map<std::string, std::string> CgiRequestHandler::parseHttpHeader(void) const
{
	std::map<std::string, std::string> headers;
	std::istringstream stream(_headers);
	std::string line;

	while (std::getline(stream, line))
	{
		if (line.empty())
			continue;

		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			key.erase(key.find_last_not_of(" \t\r\n") + 1);
			value.erase(0, value.find_first_not_of(" \t\r\n"));

			headers[key] = value;
		}
	}

	return (headers);
}

std::string CgiRequestHandler::getScriptName(void) const
{
	std::string script = _resource.substr(0, _resource.find('?'));
	if (script.find_last_of('/') != std::string::npos)
		script = script.substr(script.find_last_of('/') + 1);
	return (script);
}

std::string intToString(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}