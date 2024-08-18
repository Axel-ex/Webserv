/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:36:42 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/18 14:49:22 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiRequestHandler.hpp"

std::map<pid_t, t_client_process> CgiRequestHandler::_open_processes;

CgiRequestHandler::CgiRequestHandler(const CgiRequestHandler &other)
{
	_ch_env = other._ch_env;
	_env = other._env;
	_client_fd = other._client_fd;
	_method = other._method;
	_resource = other._resource;
	_protocol = other._protocol;
	_headers = other._headers;
	_body = other._body;
}

CgiRequestHandler &CgiRequestHandler::operator=(const CgiRequestHandler &other)
{
	if (this != &other)
	{
		_ch_env = other._ch_env;
		_env = other._env;
		_client_fd = other._client_fd;
		_method = other._method;
		_resource = other._resource;
		_protocol = other._protocol;
		_headers = other._headers;
		_body = other._body;
	}
	return (*this);
}

CgiRequestHandler::~CgiRequestHandler()
{
}

// =============================================================================
//                                    CGI HANDLER
// =============================================================================

std::string get_working_path()
{
	char temp[MAXPATHLEN];
	return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

std::string get_script(const Request &request)
{
	std::string script = "/resources" + request.getResource();
	std::string wd = get_working_path();

	script = script.substr(0, script.find('?'));
	// if (script[0] != '/')
	return (wd + script);
	// return (script);
}

std::string getContentType(const std::string &headers)
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

std::map<std::string, std::string> parseHttpHeader(const std::string &header)
{
	std::map<std::string, std::string> headers;
	std::istringstream stream(header);
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

std::string getScriptName(const std::string &url)
{
	std::string script = url.substr(0, url.find('?'));
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

void CgiRequestHandler::init_cgi_env(const Request &request)
{
	std::string script_name = getScriptName(_resource);
	std::string script_path = get_script(request);
	std::map<std::string, std::string> request_headers = parseHttpHeader(_headers);

	// IT WILL BE NEEDED TO ADD THE CGI PATH CONFIGURATION
	if (request.getMethod() == "POST")
	{
		std::stringstream out;
		out << _body.length();
		_env["CONTENT_LENGTH"] = out.str();
		_env["CONTENT_TYPE"] = getContentType(_headers);
	}
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	
	// ===================================================================
	// 							FIX THIS DATA
	_env["SCRIPT_NAME"] = script_name;
	_env["SCRIPT_FILENAME"] = script_path;
	_env["PATH_INFO"] = script_path;
	_env["PATH_TRANSLATED"] = script_path;
	// ===================================================================
	
	_env["REQUEST_URI"] = _resource;
	_env["SERVER_NAME"] = Config::getServerName();
	_env["SERVER_PORT"] = intToString(Config::getPorts()[0]);
	_env["REQUEST_METHOD"] = _method;
	_env["SERVER_PROTOCOL"] = _protocol;
	_env["REDIRECT_STATUS"] = "200";
	_env["SERVER_SOFTWARE"] = "Aether_42";
	_env["DOCUMENT_ROOT"] = _location.url;
	
	if (request_headers.find("cookie") != request_headers.end())
		_env["HTTP_COOKIE"] = request_headers["cookie"];

	for (std::map<std::string, std::string>::iterator it = request_headers.begin();
		 it != request_headers.end(); ++it)
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

void CgiRequestHandler::init_ch_env(void)
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

CgiRequestHandler::CgiRequestHandler(const Request &request, int fd)
{
	_client_fd = fd;
	_method = request.getMethod();
	_resource = request.getResource();
	_protocol = request.getProtocol();
	_headers = request.getHeaders();
	_body = request.getBody();
	init_cgi_env(request);
	init_ch_env();
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

void CgiRequestHandler::processRequest()
{
	Log::log(DEBUG, "Trying to execute: " + _env["SCRIPT_FILENAME"]);
	// int pipe[2];
	pid_t pid;
	if ((pid = fork()) < 0)
		std::cerr << "FORK ERROR." << std::endl;
	if (pid == 0)
	{
		dup2(_client_fd, STDOUT_FILENO);
		if (execve(_env["SCRIPT_FILENAME"].c_str(), _argv, _ch_env) < 0)
		{
			// ============================================================================
			// 						  TO IMPLEMENT ERROR PAGE LOGIC
			// ============================================================================
			// perror("Could not execve the file!");
			// std::string failureResponse = "HTTP/1.1 500 Internal Server Error\r\n"
			// 							  "Content-Type: text/plain\r\n\r\n"
			// 							  "Content-Length: 21\r\n\r\n"
			// 							  "Internal Server Error";
			// send(_client_fd, failureResponse.c_str(), failureResponse.length(), 0);
			// ============================================================================
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		t_client_process c_process = {_method, pid, _client_fd, clock()};
		CgiRequestHandler::_open_processes[pid] = c_process;
		
		Log::log(DEBUG, "Added c_process to _open_processes map");
		Log::log(DEBUG, "PID: " + std::to_string(pid) \
								+ ", Client FD: " \
								+ std::to_string(_client_fd) \
								+ ", Starting Time: " \
								+ std::to_string(c_process.start_time));
	}
}
