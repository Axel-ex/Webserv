/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:36:42 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/15 15:35:47 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiRequestHandler.hpp"

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

void CgiRequestHandler::init_ch_env(void)
{
}

string get_script(const Request &request)
{
	string script = request.getResource();
	std::string wd = get_working_path();

	script = script.substr(0, script.find('?'));
	if (script[0] == '/')
		return (wd + script);
	return (wd + "/" + script);
}

string	getContentType(const std::string& headers)
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

map<string, string> transform_headers(const string &headers)
{
	
}

void CgiRequestHandler::init_cgi_env(const Request &request)
{
	string script_name = request.getResource();
	script_name = script_name.substr(script_name.find_last_of('/') + 1);
	string script_path = get_script(request);
	
	// IT WILL BE NEEDED TO ADD THE CGI PATH CONFIGURATION
	if (request.getMethod() == "POST")
	{
		std::stringstream out;
		out << request.getBody().length();
		this->_env["CONTENT_LENGTH"] = out.str();
		this->_env["CONTENT_TYPE"] = getContentType(request.getHeaders());
	}
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = script_name;
	this->_env["SCRIPT_FILENAME"] = script_path;
	this->_env["PATH_INFO"] = script_path;
	this->_env["PATH_TRANSLATED"] = script_path;
	this->_env["REQUEST_URI"] = request.getResource();
	this->_env["SERVER_NAME"] = Config::getServerName();
	this->_env["SERVER_PORT"] = std::to_string(Config::getPorts()[0]);
	this->_env["REQUEST_METHOD"] = request.getMethod();
	this->_env["SERVER_PROTOCOL"] = request.getProtocol();
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_SOFTWARE"] = "BeeLoo";
}

CgiRequestHandler::CgiRequestHandler(const Request &request, int fd)
{
	_client_fd = fd;
	_method = request.getMethod();
	_resource = request.getResource();
	_protocol = request.getProtocol();
	_headers = request.getHeaders();
	_body = request.getBody();
	init_cgi_env();
	init_ch_env();
}

bool CgiRequestHandler ::_canProcess(const Request &request)
{
	std::vector<Route> routes = Config::getRoutes();
	std::string r_url = request.getResource();

	/*Verify if the requested resource exist in the previously defined locations*/
	for (size_t i = 0; i < routes.size(); i++)
	{
		// std::cout << "CGI VERIFYING...\n" << "requested url: " << r_url << "\nlocation url: " << routes[i].url << "\n";
		if (r_url.find(routes[i].url) == 0 &&
			std::find(routes[i].methods.begin(), routes[i].methods.end(), request.getMethod()) != routes[i].methods.end())
		{
			std::cout << "CGI got here...\n";
			if (routes[i].cgi_extension.empty() == false)
			{
				std::string cgi_ext = routes[i].cgi_extension;
				if (r_url.find(cgi_ext) != std::string::npos)
					return (true);
				continue;
			}
		}
	}
	// std::cout << "CGI CANNOT PROCESS...\n";
	return (false);
}

void CgiRequestHandler ::processRequest(const Request &request)
{
	(void)request;
	std::cout << "hello world from cgi" << std::endl;
	// int pipe[2];
	int pid;
	if ((pid = fork()) < 0)
		std::cerr << "FORK ERROR." << std::endl;
	if (pid == 0)
	{
		dup2(_client_fd, STDOUT_FILENO);
		if (execve(ABSOLUTE_PATH, CMD_VECTR, ENV) < 0)
		{
			std::cerr << "EXECVE ERROR" << std::endl;
		}
	}
	else
	{
	}
}
