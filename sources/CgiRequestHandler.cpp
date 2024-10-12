/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:36:42 by ebmarque          #+#    #+#             */
/*   Updated: 2024/10/12 10:32:14 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiRequestHandler.hpp"
#include <cstdio>
#include <algorithm>
#include <cstring>

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

CgiRequestHandler::CgiRequestHandler(const Request &request, int fd, const Config &config, std::map<pid_t, t_client_process> *open_processes) : _server_config(config)
{

	// ============ REQUEST INFORMATION ==============
	_method = request.getMethod();
	_resource = request.getResource();
	_protocol = request.getProtocol();
	_headers = request.getHeaders();

	_body = request.getBody();
	_client_fd = fd;
	_location = ServerTools::getBestRoute(request, config.getRoutes());
	
	if (_resource == _location.url || _resource == _location.url + "/")
			_resource = _location.url + "/" + _location.index;
	// ===============================================

	_document_root = _location.root.substr(0, _location.root.find(_location.url));
	if (!_document_root.empty() && _document_root[0] == '.')
		_document_root.erase(0, 1);

	_open_processes = open_processes;
	decode(); // --> decode hexadecimal values in the requested URI
	_extension = CgiTools::getFileExtension(_decoded_resource);
	_scriptName = getScriptName();
	_scriptPath = getScriptPath();
	_interpreter = getInterpreter();
	_request_headers = parseHttpHeader();
	_query_str = getQueryString();

	getPathInfo();
	initCgiEnv();
	initChEnv();

	Log::log(DEBUG, toString(YELLOW) + "METHOD: " + toString(RESET) + _method);
	Log::log(DEBUG, toString(YELLOW) + "SCRIPT NAME: " + toString(RESET) + _scriptName);
	Log::log(DEBUG, toString(YELLOW) + "SCRIPT INTERPRETER: " + toString(RESET) + _interpreter);
	Log::log(DEBUG, toString(YELLOW) + "SCRIPT PATH: " + toString(RESET) + _scriptPath);
	Log::log(DEBUG, toString(YELLOW) + "SCRIPT EXTENSION: " + toString(RESET) + _extension);
	Log::log(DEBUG, toString(YELLOW) + "DOCUMENT ROOT: " + toString(RESET) + getRootPath());
	Log::log(DEBUG, toString(YELLOW) + "QUERY STRING: " + toString(RESET) + _query_str);
	Log::log(DEBUG, toString(YELLOW) + "PATH INFO: " + toString(RESET) + _pathInfo);
	Log::log(DEBUG, toString(YELLOW) + "PATH TRANSLATED: " + toString(RESET) + _pathTranslated);
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

bool CgiRequestHandler::_canProcess(const Request &request, const std::vector<Route> &routes)
{
	Route best_route = ServerTools::getBestRoute(request, routes);
	if (best_route.url.empty() || best_route.cgi_extension.empty() || best_route.cgi_path.empty())
		return (false);
	else if (best_route.autoindex && best_route.index.empty())
		return (false);
	else if (request.getResource() == best_route.url || request.getResource() == best_route.url + "/")
	{
		if (best_route.index.empty())
			return (false);
		else
			return (true);
	}
	return (CgiTools::isExtensionAllowed(request.getResource(), best_route.cgi_extension));
	// return (CgiTools::isExtensionAllowed(request.getResource(), best_route.cgi_extension) || ((request.getResource() == best_route.url || request.getResource() == best_route.url + "/")));
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
		_env["CONTENT_LENGTH"] = toString(_body.size());
		_env["CONTENT_TYPE"] = getContentType(_headers);
		Log::log(DEBUG, toString(YELLOW) + "CONTENT TYPE: " + toString(RESET) + _env["CONTENT_TYPE"]);
		Log::log(DEBUG, toString(YELLOW) + "CONTENT LENGTH: " + toString(RESET) + _env["CONTENT_LENGTH"]);
	}
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SCRIPT_NAME"] = _scriptName;
	_env["SCRIPT_FILENAME"] = _scriptPath;
	_env["PATH_INFO"] = _pathInfo;
	_env["PATH_TRANSLATED"] = _pathTranslated;
	_env["REQUEST_URI"] = _resource;
	_env["REQUEST_METHOD"] = _method;
	_env["SERVER_NAME"] = _server_config.getServerName();
	_env["SERVER_PORT"] = toString(_server_config.getPorts()[0]);
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
		CgiTools::sendHttpErrorResponse(_client_fd, errno, _server_config.getErrorPath());
		return;
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
			CgiTools::sendHttpErrorResponse(_client_fd, errno, _server_config.getErrorPath());
			throw CgiError("Execve() could not execute: " + _scriptPath + " -> errno: " + toString(errno));
		}
	}
	else
	{
		close(_pipe_out[1]);
		if (_method == "POST")
		{
			int bytesWritten = 0;
			int bytesRemaining = _body.length();
			while (bytesRemaining > 0)
			{
				int bytesToWrite = std::min(BUFSIZ, bytesRemaining);
				if (write(_pipe_in[1], _body.c_str() + bytesWritten, bytesToWrite) == -1)
				{
					CgiTools::sendHttpErrorResponse(_client_fd, 500, _server_config.getErrorPath());
					close(_pipe_in[1]);
					throw CgiError("write() failed.");
				} 
				bytesWritten += bytesToWrite;
				bytesRemaining -= bytesToWrite;
			}
		}
		close(_pipe_in[1]);

		t_client_process c_process = {_method, ServerTools::getTime(), _client_fd, _pipe_out[0]};
		(*_open_processes)[pid] = c_process;
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
	size_t content_type_end_pos = headers.find("\r\n", content_type_pos);

	// Capture the entire Content-Type line, including any parameters like boundary
	std::string content_type = headers.substr(content_type_pos, content_type_end_pos - content_type_pos);
	return content_type;
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
		char decimal = CgiTools::convertHex(_decoded_resource.substr(token + 1, 2));
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
