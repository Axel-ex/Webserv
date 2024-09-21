/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:36:42 by ebmarque          #+#    #+#             */
/*   Updated: 2024/09/21 17:11:30 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiRequestHandler.hpp"

// STRUCTURE TO BE USED FOR VERIFYING TIMED-OUT PROCESSES
std::map<pid_t, t_client_process> CgiRequestHandler::_open_processes;

std::string CgiRequestHandler::getInterpreter(void) const
{
	if (std::abs((int)(_location.cgi_extension.size() - _location.cgi_path.size())) > 1)
	{
		Log::log(ERROR, "Cgi extension and path vector differs in size more than 1 unit.");
		return (_scriptPath);
	}
	if (_extension == ".cgi")
		return (_scriptPath);
	for (size_t i = 0; i < _location.cgi_extension.size(); i++)
	{
		if (_location.cgi_extension[i] == _extension)
			return (_location.cgi_path[i]);
	}
	return (_scriptPath);
}

CgiRequestHandler::CgiRequestHandler(const Request &request, int fd, Route &location)
{

	// ============ REQUEST INFORMATION ==============
	_method = request.getMethod();
	_resource = request.getResource();
	_protocol = request.getProtocol();
	_headers = request.getHeaders();
	_body = request.getBody();
	_client_fd = fd;
	_location = location;
	// ===============================================

	_document_root = _location.root.substr(0, _location.root.find(_location.url) - 1);
	if (!_document_root.empty() && _document_root[0] == '.')
		_document_root.erase(0, 1);

	decode(); // --> decode hexadecimal values in the requested URI
	_extension = getFileExtension(_decoded_resource);
	_scriptName = getScriptName();
	_scriptPath = getScriptPath();
	_interpreter = getInterpreter();
	_request_headers = parseHttpHeader();
	_query_str = getQueryString();

	getPathInfo();
	initCgiEnv();
	initChEnv();

	Log::log(DEBUG, "METHOD: " + _method);
	Log::log(DEBUG, "SCRIPT NAME: " + _scriptName);
	Log::log(DEBUG, "SCRIPT INTERPRETER: " + _interpreter);
	Log::log(DEBUG, "SCRIPT PATH: " + _scriptPath);
	Log::log(DEBUG, "SCRIPT EXTENSION: " + _extension);
	Log::log(DEBUG, "DOCUMENT ROOT: " + getRootPath());
	Log::log(DEBUG, "QUERY STRING: " + _query_str);
	Log::log(DEBUG, "PATH INFO: " + _pathInfo);
	Log::log(DEBUG, "PATH TRANSLATED: " + _pathTranslated);
}

CgiRequestHandler::~CgiRequestHandler()
{
	for (size_t i = 0; _ch_env[i] != NULL; i++)
	{
		if (_ch_env[i])
			free(_ch_env[i]);
	}
	delete[] _ch_env;
	_env.clear();
	_request_headers.clear();
	if (_argv[0])
		free(_argv[0]);
	if (_argv[1])
		free(_argv[1]);
}

bool CgiRequestHandler::_canProcess(const Request &request)
{
	std::vector<Route> routes = Config::getRoutes();
	std::string resource = request.getResource();
	std::string method = request.getMethod();

	for (size_t i = 0; i < routes.size(); i++)
	{
		if (std::find(routes[i].methods.begin(), routes[i].methods.end(), method) != routes[i].methods.end())
		{
			if (startsWith(resource, routes[i].url) && isExtensionAllowed(resource, routes[i].cgi_extension))
				return (true);
		}
	}
	return (false);
}

std::string CgiRequestHandler::getRootPath(void) const
{
	std::string wd = getWorkingPath();
	std::string root = _location.root;
	if (root[0] == '.')
		root.erase(0, 1);
	return (wd + root);
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
	_env["SCRIPT_NAME"] = _scriptName;
	_env["SCRIPT_FILENAME"] = _scriptPath;
	_env["PATH_INFO"] = _pathInfo;
	_env["PATH_TRANSLATED"] = _pathTranslated;
	_env["REQUEST_URI"] = _resource;
	_env["REQUEST_METHOD"] = _method;
	_env["SERVER_NAME"] = Config::getServerName();
	_env["SERVER_PORT"] = toString(Config::getPorts()[0]);
	_env["SERVER_PROTOCOL"] = _protocol;
	_env["REDIRECT_STATUS"] = "200";
	_env["SERVER_SOFTWARE"] = "Aether_42";
	_env["DOCUMENT_ROOT"] = getRootPath();
	_env["QUERY_STRING"] = _query_str;

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
	_argv[0] = strdup(_interpreter.c_str());
	_argv[1] = strdup(_scriptPath.c_str());
	_argv[2] = NULL;
}

void CgiRequestHandler::initChEnv(void)
{
	int i = 0;
	std::map<std::string, std::string>::iterator it = _env.begin();

	_ch_env = new char *[_env.size() + 1];
	if (!_ch_env)
		throw CgiError("Error allocating memory for CGI environment variables.");

	for (; it != _env.end(); it++, i++)
	{
		std::string variable = it->first + "=" + it->second;
		_ch_env[i] = strdup(variable.c_str());
	}
	_ch_env[i] = NULL;
}

// ==========================================================================================
// 									PROCESSING REQUESTED SCRIPT
// ==========================================================================================

void CgiRequestHandler::processRequest()
{
	pid_t pid;
	if (open(_scriptPath.c_str(), F_OK) == -1)
	{
		sendHttpErrorResponse(_client_fd, errno);
		return ;
	}
	if (pipe(_pipe_in) < 0 || pipe(_pipe_out) < 0)
		throw CgiError("Error in pipe().");
	if ((pid = fork()) < 0)
		throw CgiError("Error in fork().");
	if (pid == 0)
	{
		close(_pipe_out[0]);
		dup2(_pipe_out[1], STDOUT_FILENO);
		close(_pipe_out[1]);

		close(_pipe_in[1]);
		dup2(_pipe_in[0], STDIN_FILENO);
		close(_pipe_in[0]);

		if (execve(_argv[0], _argv, _ch_env) < 0)
		{
			sendHttpErrorResponse(_client_fd, errno);
			throw CgiError("Execve() could not execute: " + _scriptPath + " -> errno: " + toString(errno));
		}
	}
	else
	{
		close(_pipe_out[1]);
		if (_method == "POST")
			send(_pipe_in[0], _body.c_str(), _body.length(), 0);
		close(_pipe_in[0]);

		t_client_process c_process = {_method, clock(), _client_fd, _pipe_out[0]};
		CgiRequestHandler::_open_processes[pid] = c_process;
	}
}

// =========================================================================================
//									AUXILIAR FUNCTIONS AND METHODS
// =========================================================================================

std::string CgiRequestHandler::getQueryString(void)
{
	std::string query("");
	if (_resource.find("?") != std::string::npos)
		query = _resource.substr(_resource.find("?") + 1);
	return (query);
}

std::string CgiRequestHandler::getWorkingPath(void) const
{
	char temp[MAXPATHLEN];
	return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

std::string CgiRequestHandler::getScriptPath(void) const
{
	std::string script = _document_root + _decoded_resource;
	std::string wd = getWorkingPath();

	script = script.substr(0, script.find('?'));
	script = wd + script;
	if (!script.empty() && script[script.length() - 1] == '/')
		script = script.substr(0, script.length() - 1);

	size_t scriptNamePos = script.rfind(_scriptName);
	if (scriptNamePos != std::string::npos)
		script = script.substr(0, scriptNamePos + _scriptName.length());
	return (script);
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
	std::string uri = _decoded_resource;

	std::size_t query_pos = uri.find('?');
	if (query_pos != std::string::npos)
		uri = uri.substr(0, query_pos);

	std::size_t fragment_pos = uri.find('#');
	if (fragment_pos != std::string::npos)
		uri = uri.substr(0, fragment_pos);

	std::size_t ext_pos = uri.rfind(_extension);
	if (ext_pos != std::string::npos)
	{
		if (ext_pos + _extension.length() == uri.length() ||
			uri[ext_pos + _extension.length()] == '/')
		{
			std::size_t last_slash_pos = uri.rfind('/', ext_pos);
			if (last_slash_pos != std::string::npos)
				return (uri.substr(0, ext_pos + _extension.length()));
		}
	}
	return (uri.substr(uri.rfind('/') + 1));
}

std::string CgiRequestHandler::decode()
{
	_decoded_resource = _resource;
	size_t token = _decoded_resource.find("%");
	while (token != std::string::npos)
	{
		if (_decoded_resource.length() < token + 2)
			break;
		char decimal = convertHex(_decoded_resource.substr(token + 1, 2));
		_decoded_resource.replace(token, 3, toString(decimal));
		token = _decoded_resource.find("%");
	}
	return (_decoded_resource);
}

void CgiRequestHandler::getPathInfo(void)
{
	std::string uri = _decoded_resource.substr(0, _decoded_resource.find('?'));
	std::string pathInfo = uri.substr(uri.find(_scriptName) + _scriptName.length());
	_pathInfo = pathInfo.empty() ? "/" : pathInfo;
	_pathTranslated = getRootPath() + _pathInfo;
}
